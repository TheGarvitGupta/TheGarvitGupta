# Master storage
127.0.0.1:10000, Master
# Heartbeat Server
127.0.0.1:15000, Heartbeat
# Front end load balancer
127.0.0.1:20000, Load Balancer
# Storage group 
127.0.0.1:5000, Storage - G1
127.0.0.1:5001, Storage - G1
127.0.0.1:5002, Storage - G1
# Storage group
127.0.0.1:6000, Storage - G2
127.0.0.1:6001, Storage - G2
127.0.0.1:6002, Storage - G2
# Storage group
127.0.0.1:7000, Storage - G3
127.0.0.1:7001, Storage - G3
127.0.0.1:7002, Storage - G3
# Front end units
127.0.0.1:8000, FE
127.0.0.1:8001, FE
127.0.0.1:8002, FE
#heartbeatg1-10:g2-20:g3-30
# SMTP server
127.0.0.1:30000, SMTP