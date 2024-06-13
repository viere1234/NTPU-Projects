// Import required modules
const http = require('http');
const express = require('express');
const socketio = require('socket.io');
const readline = require('readline');
const os = require('os');

// Initialize the express app
const app = express();

// Create a basic HTTP server using express
const server = http.createServer(app);

// Initialize socket.io
const io = socketio(server);

// Serve static files from the 'public' directory
app.use(express.static('public'));

// Store necessary information
let clients = {};
let calledNumbers = [];
let gameRunning = false

// Function to generate a random number between 1 and 25
function generateRandomNumber() {
  return Math.floor(Math.random() * 25) + 1;
}

// Function to check if a number is already called
function isNumberCalled(number) {
  return calledNumbers.includes(number);
}

// Create interface for reading commands from terminal
const rl = readline.createInterface({
  input: process.stdin,
  output: process.stdout
});

// Event listener for when a client connects
io.on('connection', (socket) => {
  console.log('New client connected:', socket.id);

  socket.emit('clientId', socket.id);

  // Set a default name for the client
  clients[socket.id] = {
    name: `User${Object.keys(clients).length + 1}`,
    ready: false,
    score: 0
  };


  // Send the list of connected clients to all clients
  io.emit('updateClients', clients);

  // Send the called numbers to the new client
  socket.emit('updateCalledNumbers', calledNumbers);

  // Event listener for when a client sets their name
  socket.on('setName', (name) => {
    console.log('Client:', socket.id, 'is setting name:', name)
    clients[socket.id].name = name;
    socket.broadcast.emit('updateClients', clients);
  });

  // Event listener for when a client sets their readiness status
  socket.on('setReady', () => {
    console.log('Player:', clients[socket.id].name, 'is ready')
    clients[socket.id].ready = true;
    // Broadcast readiness status to all clients except the sender
    socket.broadcast.emit('updateReadiness', socket.id);
    // Check if all clients are ready to start the game
    checkAllReady();
  });

  // Event listener for when a client sends their score
  socket.on('sendScore', (score) => {
    console.log('Player:', clients[socket.id].name, 'update score:', score)
    clients[socket.id].score = score;
    // Broadcast the updated scores to all clients
    io.emit('updateClients', clients);
  });

  // Event listener for when a client disconnects
  socket.on('disconnect', () => {
    console.log('Client disconnected:', socket.id);

    // Remove the client from the list of clients
    delete clients[socket.id];

    // Send the updated list of connected clients to all clients
    io.emit('updateClients', clients);
  });
});

// Function to check if all clients are ready to start the game
function checkAllReady() {
  const allReady = Object.values(clients).every(client => client.ready);
  if (allReady) {
    console.log('All players are ready. Starting game...');
    calledNumbers = [];
    gameRunning = true
    io.emit('startGame');
  }
}

// Function to restart the game and clear all previous game informations
function restartGame() {
  console.log('Current Game Result:')
  for (const clientId in clients) {
    const client = clients[clientId];
    console.log(`Name: ${client.name}, Score: ${client.score}`);
  }
  console.log('Restarting game...');
  calledNumbers = [];
  gameRunning = false;
  for (let key in clients) {
    clients[key].ready = false;
  }
  io.emit('updateClients', clients);
  io.emit('restartGame');
}

// Function to process commands from the terminal
function processCommand(command) {
  switch (command.trim()) {
    case 'start':
      console.log('Starting game...');
      calledNumbers = [];
      gameRunning = true
      io.emit('startGame');
      break;
    case 'restart':
      restartGame();
      break;
    case 'next':
      if (gameRunning === false) {
        console.log('Game hasn\'t started, start the game or wait for all players are ready.');
        break;
      }
      if (calledNumbers.length === 25) {
        console.log('Current game is ended');
        restartGame();
        break;
      }
      let number;
      do {
        number = generateRandomNumber();
      } while (isNumberCalled(number));
      console.log('Next number is', number);
      calledNumbers.push(number);
      io.emit('newNumber', number);
      io.emit('updateCalledNumbers', calledNumbers);
      break;
    default:
      console.log('Invalid command.');
      break;
  }
}

// Event listener for when a line is entered in the terminal
rl.on('line', (input) => {
  processCommand(input);
});

// Get local IP address
const networkInterfaces = os.networkInterfaces();
const addresses = [];
for (const networkTnterface of Object.values(networkInterfaces)) {
  for (const { address, family, internal } of networkTnterface) {
    if (family === 'IPv4' && !internal) {
      addresses.push(address);
    }
  }
}

// Start the server
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`Server running on:`);
  for (const address of addresses) {
    console.log(`- http://${address}:${PORT}`);
  }
});
