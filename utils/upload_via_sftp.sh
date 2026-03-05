#!/bin/bash

# Load the configuration file
CONFIG_FILE="sftp_config.cfg"

if [ ! -f "$CONFIG_FILE" ]; then
    echo "[ERROR] Configuration file $CONFIG_FILE not found."
    exit 1
fi

# Read configuration settings
source $CONFIG_FILE

# Verify required settings are loaded
if [[ -z "$SFTP_HOST" || -z "$SFTP_USER" || -z "$SFTP_PASS" || -z "$SOURCE_FILE" || -z "$SFTP_REMOTE_DIR" ]]; then
    echo "[ERROR] Missing configuration settings in $CONFIG_FILE. Check SFTP_HOST, SFTP_USER, SFTP_PASS, SOURCE_FILE, and SFTP_REMOTE_DIR."
    exit 1
fi

echo "[INFO] Configuration loaded successfully."
echo "[INFO] SFTP Host: $SFTP_HOST"
echo "[INFO] SFTP User: $SFTP_USER"
echo "[INFO] SFTP Port: ${SFTP_PORT:-22}"
echo "[INFO] Source File: $SOURCE_FILE"
echo "[INFO] Remote Directory: $SFTP_REMOTE_DIR"

# Validate source file
if [ ! -f "$SOURCE_FILE" ]; then
    echo "[ERROR] Source file $SOURCE_FILE does not exist."
    exit 1
fi

# Execute SFTP using configuration settings
echo "[INFO] Starting SFTP upload..."
sshpass -p "$SFTP_PASS" sftp -oBatchMode=no -P ${SFTP_PORT:-22} $SFTP_USER@$SFTP_HOST <<EOF
cd $SFTP_REMOTE_DIR || { echo "[ERROR] Failed to change directory to $SFTP_REMOTE_DIR on remote server."; exit 1; }
put $SOURCE_FILE || { echo "[ERROR] Failed to upload $SOURCE_FILE to $SFTP_REMOTE_DIR."; exit 1; }
bye
EOF

if [ $? -ne 0 ]; then
    echo "[ERROR] SFTP upload failed. Check connection details and credentials."
    exit 1
fi

echo "[SUCCESS] File successfully uploaded to $SFTP_HOST:$SFTP_REMOTE_DIR"
