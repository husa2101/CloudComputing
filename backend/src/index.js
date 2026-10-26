const express = require('express');
const bodyParser = require('body-parser');
const { Pool } = require('pg');

const app = express();
const port = 3000;


app.use(bodyParser.json());

// These use environment variables
const pool = new Pool({
    host: process.env.DB_HOST 
    user: process.env.DB_USER 
    password: process.env.DB_PASS 
    database: process.env.DB_NAME 
    port: 5432,
});

// Create the Users table if it doesn't exist
pool.query(`
    CREATE TABLE IF NOT EXISTS Users (
        id SERIAL PRIMARY KEY,
        username VARCHAR(50) UNIQUE NOT NULL,
        password VARCHAR(50) NOT NULL
    )
`).catch(err => console.error("DB Init Error:", err));

// The REST API Endpoint for Login
app.post('/login', async (req, res) => {
    const { username, password } = req.body;

    if (!username || !password) {
        return res.status(400).json({ status: "error", message: "Username and password required" });
    }

    try {
        // Check if user exists
        const result = await pool.query('SELECT * FROM Users WHERE username = $1', [username]);

        if (result.rows.length > 0) {
            // User exists, check password
            const user = result.rows[0];
            if (user.password === password) {
                res.json({ status: "success", message: "Login successful" });
            } else {
                res.json({ status: "error", message: "Incorrect password" });
            }
        } else {
            // User does not exist, let's auto-register them for this assignment
            await pool.query('INSERT INTO Users (username, password) VALUES ($1, $2)', [username, password]);
            res.json({ status: "success", message: "User registered and logged in" });
        }
    } catch (err) {
        console.error(err);
        res.status(500).json({ status: "error", message: "Database error" });
    }
});

app.listen(port, () => {
    console.log(`Auth API Microservice running on http://localhost:${port}`);
});
