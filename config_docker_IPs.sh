#!/bin/bash

# Define your IP addresses here
IP_BROKER="broker"
IP_TEAM="team"
IP_GAMECARD="gamecard"

# broker config
sed -i "s/IP_BROKER=.*/IP_BROKER=$IP_BROKER/" ./Broker/broker.config

# team config
sed -i "s/IP=.*/IP=$IP_TEAM/" ./Team/team.config
sed -i "s/IP_BROKER=.*/IP_BROKER=$IP_BROKER/" ./Team/team.config

# gamecard config
sed -i "s/IP_GAMECARD=.*/IP_GAMECARD=$IP_GAMECARD/" ./GameCard/gamecard.config
sed -i "s/IP_BROKER=.*/IP_BROKER=$IP_BROKER/" ./GameCard/gamecard.config

# gameboy config
sed -i "s/IP_GAMECARD=.*/IP_GAMECARD=$IP_GAMECARD/" ./GameBoy/gameboy.config
sed -i "s/IP_TEAM=.*/IP_TEAM=$IP_TEAM/" ./GameBoy/gameboy.config
sed -i "s/IP_BROKER=.*/IP_BROKER=$IP_BROKER/" ./GameBoy/gameboy.config