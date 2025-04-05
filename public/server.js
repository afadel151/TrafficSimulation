const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const cors = require('cors');
const { spawn } = require('child_process');

const app = express();
app.use(cors());
app.use(express.json()); // To parse JSON bodies

const server = http.createServer(app);

app.post('/run-osm-web-wizard', (req, res) => {
 
  const pythonProcess = spawn('python', ['C:\\sumo\\tools\\osmWebWizard.py']);

  pythonProcess.stdout.on('data', (data) => {
    console.log(`stdout: ${data}`);
  });

  pythonProcess.stderr.on('data', (data) => {
    console.error(`stderr: ${data}`);
  });

  pythonProcess.on('close', (code) => {
    console.log(`child process exited with code ${code}`);
    res.send(`Process finished with code ${code}`);
  });

  const netConvertProcess = spawn('python', ['.\\net2geojson.py','-n',`.\\${req.body.path}\\osm.net.xml`,'-o',`${req.body.path}.osm.net.geojson`]);
  netConvertProcess.stdout.on('data', (data) => {
    console.log(`stdout: ${data}`);
  });
  netConvertProcess.on('close', (code) => {
    console.log(`child process exited with code ${code}`);
  });
});
app.post('/convert-network', (req, res) => {
  console.log('converting network ',req.body.path);
  const netConvertProcess = spawn('python', ['.\\net2geojson.py','-n',`.\\${req.body.path}\\osm.net.xml`,'-o',`${req.body.path}.osm.net.geojson`]);
  netConvertProcess.stdout.on('data', (data) => {
    console.log(`stdout: ${data}`);
  });
  netConvertProcess.on('close', (code) => {
    console.log(`child process exited with code ${code}`);
    res.json({
      status: 'success',
      message: 'Network converted successfully',
    });
  });
});

const PORT = process.env.PORT || 3333;
server.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
  console.log(`SUMO_HOME is set to: ${process.env.SUMO_HOME}`);

});

console.log('Fadel');