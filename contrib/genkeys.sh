#!/bin/bash
# generate genensis keys
COIN=$1

openssl ecparam -genkey -name secp256k1 -out ${COIN}-alertkey.pem
openssl ec -in ${COIN}-alertkey.pem -text > ${COIN}-alertkey.hex
openssl ecparam -genkey -name secp256k1 -out ${COIN}-testnetalert.pem
openssl ec -in ${COIN}-testnetalert.pem -text > ${COIN}-testnetalert.hex
openssl ecparam -genkey -name secp256k1 -out ${COIN}-genesiscoinbase.pem
openssl ec -in ${COIN}-genesiscoinbase.pem -text > ${COIN}-genesiscoinbase.hex
