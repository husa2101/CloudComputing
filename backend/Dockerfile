# Use the official lightweight Node 18 image
FROM node:18-alpine

# Set the working directory inside the container
WORKDIR /app

# Copy package.json and package-lock.json first to cache dependencies
COPY package*.json ./

# Install the dependencies
RUN npm install

# Copy the rest of your application code (like the src/ folder)
COPY . .

# Expose the port your Express server runs on
EXPOSE 3000

# The command to start the backend
CMD ["node", "src/index.js"]

