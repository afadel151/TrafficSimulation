const express = require('express');
const http = require('http');
const cors = require('cors');
const path = require('path');
const fs = require('fs');
const { spawn } = require('child_process');
const { createClient } = require('@supabase/supabase-js')

const url = 'https://rpwnfrlgbvebtitmlfqa.supabase.co'
const key = 'eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6InJwd25mcmxnYnZlYnRpdG1sZnFhIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NDI5MDU5NjIsImV4cCI6MjA1ODQ4MTk2Mn0.uuuxAfJ4wr8f31YU3F_kozvn92CMlk4lJG13a2L7-rY'

const supabase = createClient(url, key)
const app = express();
app.use(cors());
app.use(express.json());

const server = http.createServer(app);

app.post('/run-osm-web-wizard', async (req, res) => {
  const form = req.body.form;

  const pythonProcess = spawn('py', ['C:\\sumo\\tools\\osmWebWizard.py', `--name=${form.name}`]);

  pythonProcess.stdout.on('data', (data) => {
    console.log(`stdout: ${data}`);
    if (data == 'Written configuration to \'osm.sumocfg\'') {
      console.log('Configuration file written successfully');
    }

  });
  pythonProcess.stderr.on('data', (data) => {
    console.error(`stderr: ${data}`);

  });
  pythonProcess.on('close', (code) => {
    console.log(`child process exited with code ${code}`);
    res.send(`Process finished with code ${code}`);
  });
  const result = await supabase.from('simulations').insert({
    path: form.name,
    name: form.name,
    user_id: form.userId,
    user_name: form.userName,
    user_email: form.userEmail,
    duration: form.duration,
    vehicles: form.vehicles,
    speed: form.speed,
    cache_v: form.cacheVehicle,
    cache_rsu: form.cacheRSU,
    tx_v: form.txPowerVehicle,
    tx_rsu: form.txPowerRSU,
    tx_mbs: form.txPowerMBS
  })
  console.log(result);

});
app.post('/convert-network', (req, res) => {
  console.log('converting network ', req.body);
  const inputPath = path.join('.', req.body.path, 'osm.net.xml');
  const outputPath = path.join('.', req.body.path, 'osm.net.geojson');

  if (fs.existsSync(outputPath)) {
    console.log('Output file already exists');
    return res.json({
      status: 'success',
      message: 'File already exists, no conversion needed',
      existing: true
    });
  }
  if (!fs.existsSync(inputPath)) {
    console.log('Input file does not exist');
    return res.status(400).json({
      status: 'error',
      message: 'Input file does not exist'
    });
  }
  const netConvertProcess = spawn('py', ['.\\net2geojson.py', '-n', inputPath, '-o', outputPath]);
  netConvertProcess.stdout.on('data', (data) => {
    console.log(`stdout: ${data}`);
  });

  netConvertProcess.stderr.on('data', (data) => {
    console.error(`stderr: ${data}`);
  });
  netConvertProcess.on('close', (code) => {
    console.log(`child process exited with code ${code}`);
    if (code === 0) {
      res.json({
        status: 'success',
        message: 'Network converted successfully',
      });
    } else {
      res.status(500).json({
        status: 'error',
        message: 'Conversion failed'
      });
    }
  });
});
app.post('/generate-ini', async (req, res) => {
  try {
    let rsu_position = req.body.rsu_position;
    let mbs_positions = req.body.mbs_positions;
    mbs_positions = ArrtoGeoJSON(mbs_positions);
    rsu_position = toGeoJSON(rsu_position);
    try {
      await supabase
        .from('simulations')
        .update({
          rsu_position: rsu_position,
          mbs_positions: mbs_positions
        })
        .eq('id', req.body.simulation.id);

    } catch (error) {
      console.log('error');

    }
    const template = fs.readFileSync('omnetpp.template.ini', 'utf8');
    if (fs.existsSync('omnetpp.ini')) {
      fs.unlinkSync('omnetpp.ini');
      console.log('Removed existing omnetpp.ini');
    }
    console.log('Writing omnetpp.ini');
    const config = template.replace('${duration}', req.body.simulation.duration)
      .replace('${tx_rsu}', req.body.simulation.tx_rsu)
      .replace('${tx_mbs}', req.body.simulation.tx_mbs)
    fs.writeFileSync('omnetpp.ini', config);
    // C:\\...\\...
    console.log(success);

  } catch (error) {
    console.error('Error writing INI file:', error);
    return res.status(500);
  }
})
const PORT = process.env.PORT || 3333;
server.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
  console.log(`SUMO_HOME is set to: ${process.env.SUMO_HOME}`);

});

console.log('Fadel');

function ArrtoGeoJSON(coordinatesArray) {
  if (coordinatesArray.length === 0) {
    return [];
  }
  return {
    type: "FeatureCollection",
    features: coordinatesArray.map(coord => ({
      type: "Feature",
      properties: {},
      geometry: {
        type: "Point",
        coordinates: coord
      }
    }))
  };
}
function toGeoJSON(coodinates) {
  if (coodinates.length === 0) {
    return [];
  }
  return {
    type: "Feature",
    properties: {},
    geometry: {
      type: "Point",
      coordinates: coodinates
    }
  }
}