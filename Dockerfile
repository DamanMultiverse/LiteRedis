# Base image with required build environment
FROM ubuntu:22.04

# Install build dependencies (C++ compiler and Make)
RUN apt-get update && apt-get install -y g++ make

# Set the working directory inside the container
WORKDIR /app

# Copy source code and build files to the container
COPY . .

# Compile the application using the Makefile
RUN make

# Expose the default TCP port for LiteRedis client connections
EXPOSE 6380

# Start the LiteRedis server on container startup
CMD ["./LiteRedis"]