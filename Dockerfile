# Stage 1: Build Environment
FROM ubuntu:22.04 AS builder
RUN apt-get update && apt-get install -y g++ make
WORKDIR /app
COPY . .
# Compile the application
RUN make

# Stage 2: Lightweight Production Environment
FROM ubuntu:22.04
WORKDIR /app
# Copy ONLY the compiled binary from the builder stage
COPY --from=builder /app/LiteRedis .

EXPOSE 6380
CMD ["./LiteRedis"]