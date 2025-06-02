const express = require('express');
const cors = require('cors');
const session = require('express-session');
const nodemailer = require('nodemailer');
const jwt = require('jsonwebtoken');
const { Service } = require('./Service');
const { LoggingService } = require('./LoggingService');
const { Sequelize } = require('sequelize');
const UserOTPVerification = require('./models/UserOTPVerification.js');
const ACCESS_TOKEN_SECRET = 'Vp!rzE(`3GyA&2rP16#{';
const REFRESH_TOKEN_SECRET = 'Vp!rzE(`3GyA!2rP16#{';

// --- Sequelize Initialization ---
const sequelize = new Sequelize('HomeBakers_International', 'postgres', 'postgres', {
  host: 'localhost',
  dialect: 'postgres',
  port: 5432,
  logging: false
});

const UserModel = require('./models/User')(sequelize);
const RecipeModel = require('./models/Recipe')(sequelize);
const OperationModel = require('./models/Operation')(sequelize);
const UserOTPVerifictionModel = require('./models/UserOTPVerification.js')(sequelize);

UserModel.hasMany(RecipeModel, { foreignKey: 'userUsername', sourceKey: 'username' });
RecipeModel.belongsTo(UserModel, { foreignKey: 'userUsername', targetKey: 'username' });
UserModel.hasMany(OperationModel, { foreignKey: 'userUsername', sourceKey: 'username' });
OperationModel.belongsTo(UserModel, { foreignKey: 'userUsername', sourceKey: 'username' });

// --- For Mailing OTP ---
const SERVER_EMAIL = 'marius.info.bin@gmail.com';
let transporter = nodemailer.createTransport({
  host: 'smtp.gmail.com',
  auth: {
    user: 'marius.info.bin@gmail.com',
    pass: 'xtfh twnk gzli njml'
  }
});
// --- OTP STUFF ---
UserModel.hasMany(UserOTPVerifictionModel, { foreignKey: 'username', sourceKey: 'username' });
UserOTPVerifictionModel.belongsTo(UserModel, { foreignKey: 'username', targetKey: 'username' });
async function sendOTPVerification({ username, email }, res) {
  try {
    const otp = `${Math.floor(1000 + Math.random() * 9000)}`;
    const mailOptions = {
      from: SERVER_EMAIL,
      to: email,
      subject: 'Verify your sign-in',
      html: `<p>Enter <b>${otp}</b> on Homebakers International to complete your sign-in.</p><p>This code <b>expires in 5 minutes</b></p>`
    }
    await UserOTPVerifictionModel.upsert({
      username: username,
      email: email,
      otp: otp,
      createdAt: Date.now(),
      expiresAt: Date.now() + 300000
    });
    await transporter.sendMail(mailOptions);
    res.status(200).send('OTP verification email sent!');
  } catch (error) {
    console.log(username);
    console.log(error);
    res.status(400).send('Failed to send OTP email');
  }
}

async function initializeDatabaseConnection() {
    try {
    await sequelize.authenticate();
    console.log('PostgreSQL database connection has been established successfully.');
    await sequelize.sync({ alter: true });
    console.log('All models were synchronized successfully.');
  } catch (error) {
    console.error('Unable to connect to the database or sync models:', error);
  }
}

// --- End Sequelize Initialization ---

const app = express();
const service = new Service(RecipeModel, UserModel);
const loggingService = new LoggingService(OperationModel);
const port = 8080;

// CORS configuration
const corsOptions = {
  origin: (origin, callback) => {
    if (!origin || ['http://localhost:7088', 'https://localhost:7088'].includes(origin)) {
      return callback(null, true);
    }
    callback(new Error(`CORS policy: Origin ${origin} not allowed`));
  },
  methods: ['GET', 'POST', 'PATCH', 'DELETE', 'OPTIONS'],
  allowedHeaders: ['Content-Type', 'Authorization'],
  credentials: true
};

app.options(['/verifyOTP', '/recipes', '/recipe/new', '/recipe/edit/:id', '/register', '/login', '/logout'], cors(corsOptions));
app.use(['/verifyOTP', '/recipes', '/recipe/new', '/recipe/edit/:id', '/register', '/login', '/logout'], cors(corsOptions));

// Middleware Setup 

app.use(express.json());
app.use(session({
  secret: 'Vp!rzE(`3GyA&2rP16#{',
  resave: false,
  saveUninitialized: false,
  cookie: {
    secure: false,
    httpOnly: true,
    sameSite: 'lax',
  }
}));

// --- Authentication Middleware ---
const isAuthenticated = (req, res, next) => {
  const authHeader = req.headers['authorization'];
  const token = authHeader && authHeader.split(' ')[1];
  if (token == null) {
    res.status(401).send('Unauthorized: You must be logged in to access this resource.');
  }
  jwt.verify(token, ACCESS_TOKEN_SECRET, (err, username) => {
    if (err) {
      res.status(403).send('Unauthorized: That token\'s no longer valid.');
    }
    req.user = { username: username };
    next();
  });
};

// POST /verifyOTP
app.post('/verifyOTP', async (req, res) => {
  const { username, otp } = req.body;
  if (!username || !otp) {
    return res.status(403).send("Stop tryna hack me bruv");
  }
  try {
    const entry = await UserOTPVerifictionModel.findOne({
      where: {
        username: username,
        otp: otp
    }
    });
    if (!entry) {
      return res.status(401).json({ error: 'Invalid user or OTP. Try harder.' });
    }
    if (Date.now() > entry.expiresAt) {
      return res.status(401).json({ error: 'OTP code expired.' });
    }
    await entry.destroy();
  } catch (err) {
    console.error('DB lookup failed:', err);
    return res.status(500).json({ error: 'Server error. You suck.' });
  }
  req.session.user = { username: username };
  return res.status(200).json({ accessToken: jwt.sign({ username: username }, ACCESS_TOKEN_SECRET, { expiresIn: '5m' }) });
});

// DELETE /recipes?id=<id>
app.delete('/recipes', isAuthenticated, async (req, res) => {
  const { id } = req.query;
  if (!id) {
    return res.status(400).send('Error deleting the recipe: no id provided in query params');
  }
  const username = req.user.username.username;

  try {
    const success = await service.deleteRecipeById(parseInt(id, 10), username);
    if (success) {
      loggingService.addOperation(username, "DELETE", new Date());
      return res.sendStatus(204);
    }
    return res.status(404).send('Error deleting the recipe: no recipe found with the provided id');
  } catch (err) {
    return res.status(400).send(`Error deleting recipe: ${err.message}`);
  }
});

// GET /recipes?cuisine=&doughType=&flourType=&kneadingMethod=&sortBy=&ascending=
app.get('/recipes', isAuthenticated, async (req, res) => {
  const params = req.query;
  const hasFilter = Object.values(params).some(val => val && val !== '');

  try {
    await service.loadRecipes();

    if (hasFilter) {
      service.filterByCuisine(params.cuisine || '');
      service.filterByDoughType(params.doughType || '');
      service.filterByFlourType(params.flourType || '');
      service.filterByKneadingMethod(params.kneadingMethod || '');
      service.sortFilteredBy(params.sortBy || '', params.ascending === 'True');
    }

    const data = service.getFiltered();
    console.log(data)
    return res.status(200).json(data);
  } catch (err) {
    console.error('Error in GET /recipes:', err);
    return res.status(500).send(`Server Error: ${err.message}`);
  }
});

// POST /recipe/new
app.post('/recipe/new', isAuthenticated, async (req, res) => {
  const obj = req.body;
  if (!obj || typeof obj !== 'object') {
    return res.status(400).send('Frontend Exception: Invalid JSON');
  }
  const username = req.user.username.username;
  console.log(req.user);

  try {
    const recipe = obj;
    await service.addRecipe(recipe, username);
    loggingService.addOperation(username, "CREATE", new Date());
    return res.sendStatus(201);
  } catch (err) {
    console.error('Error in POST /recipe/new:', err);
    return res.status(400).send(`Error creating recipe: ${err.message}`);
  }
});

// PATCH /recipe/edit/:id
app.patch('/recipe/edit/:id', isAuthenticated, async (req, res) => {
  const id = parseInt(req.params.id, 10);
  if (isNaN(id)) {
    return res.status(400).send('Invalid recipe ID in path');
  }

  const obj = req.body;
  if (!obj || typeof obj !== 'object') {
    return res.status(400).send('Frontend Exception: Invalid JSON');
  }
  const username = req.user.username.username;

  try {
    const newRecipe = obj;
    const success = await service.updateRecipeById(id, newRecipe, username);
    if (success) {
      loggingService.addOperation(username, "UPDATE", new Date());
      return res.sendStatus(200);
    }
    return res.status(404).send('Error updating the recipe: no recipe found with the provided id');
  } catch (err) {
    return res.status(400).send(`Error updating the recipe: ${err.message}`);
  }
});

// POST /register
app.post('/register', async (req, res) => {
  const { username, email, password } = req.body;

  // Ambatu validate
  if (!username || !password) {
    return res.status(400).send('Username and password are required');
  }
  if (typeof username !== 'string' || /^[^a-zA-Z0-9]/.test(username)) {
      return res.status(400).send('Username must be alphanumeric');
  }
  if (typeof password !== 'string') {
      return res.status(400).send('Received password not identified as a string. U tryna hack me bra?');
  }
  if (typeof email !== 'string' || !/^[A-Za-z0-9._%+-]+@[A-Za-z0-9.-]+\.[A-Za-z]{2,}$/.test(email)) {
      return res.status(400).send('Received email is not a valid email. Stawp');
  }

  try {
    const existingUser = await UserModel.findOne({ where: { username } });
    if (existingUser) {
      return res.status(409).send('Username already exists');
    }

    // mark as unverified in the database then verify later but too lazy for that
    const newUser = await UserModel.create({ username, password });
    return await sendOTPVerification({ username, email }, res);
  } catch (error) {
    console.error('Error during registration: ', error);
    if (error.name === 'SequelizeValidationError') {
        return res.status(400).send(error.errors.map(e => e.message).join(', '));
    }
    res.status(500).send('Internal server error during registration');
  }
});

// POST /login
app.post('/login', async (req, res) => {
  const { username, email, password } = req.body;

  if (!username || !password) {
    return res.status(400).send('Username and password are required.');
  }

  try {
    const user = await UserModel.findOne({ where: { username } });
    if (!user) {
      return res.status(401).send('Invalid username or password.');
    }
    if (user.password !== password) {
      return res.status(401).send('Invalid username or password.');
    }
    if (user.email !== email) {
      return res.status(401).send('Invalid email.');
    }
    return await sendOTPVerification({ username, email }, res);
  } catch (error) {
    console.error('Error during login:', error);
    return res.status(500).send('Internal server error during login.');
  }
});

// POST /logout
app.post('/logout', (req, res) => {
  req.session.destroy(err => {
    if (err) {
      return res.status(500).send('Could not log out, please try again.');
    }
    res.clearCookie('connect.sid');
    res.status(200).send('Logout successful');
  });
});

initializeDatabaseConnection().then(() => app.listen(port, () => {
  console.log(`Server listening on port ${port}`);
}));
