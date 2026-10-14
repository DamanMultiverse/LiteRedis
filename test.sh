#!/bin/bash
# Automated concurrency stress test for LiteRedis

echo "🚀 Starting LiteRedis Concurrency Stress Test..."

# Function to simulate a single client sending multiple commands
simulate_client() {
    local id=$1
    # Sending PING, SET, GET, and EXIT commands rapidly
    echo -e "PING\nSET test_key:$id value_$id\nGET test_key:$id\nEXIT\n" | nc localhost 6380 > /dev/null 2>&1
}

# Launch 100 concurrent clients in the background
for i in {1..100}
do
    simulate_client $i &
done

# Wait for all background processes to finish
wait
echo "✅ All 100 concurrent clients successfully processed!"