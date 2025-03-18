const express = require('express');
const http = require('http');
const { Server } = require('socket.io');
const cors = require('cors');
const { spawn } = require('child_process');

const app = express();
app.use(cors());
app.use(express.json()); // To parse JSON bodies

const server = http.createServer(app);
const io = new Server(server, {
  cors: {
    origin: '*', // Allow all origins (update in production)
  },
});
app.post('/run-osm-web-wizard', (req, res) => {
 
  const pythonProcess = spawn('python', ['C:\\Users\\FADEL\\Documents\\GitHub\\TrafficSimulation\\public\\sumo\\tools\\osmWebWizard.py']);

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
});

const PORT = process.env.PORT || 3333;
server.listen(PORT, () => {
  console.log(`Server running on http://localhost:${PORT}`);
  console.log(`SUMO_HOME is set to: ${process.env.SUMO_HOME}`);

});

console.log('Fadel');