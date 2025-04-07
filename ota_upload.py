Import("env")

env.Replace(
    UPLOADERFLAGS=[
        "--debug",
        "--progress",
        "--timeout=10",
        "--auth=123456",  # mesma senha do ArduinoOTA
        "--ip=%s" % env.get("UPLOAD_PORT"),
        "--port=3232",  # porta OTA
    ]
)