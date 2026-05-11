#include "iwr6843_controller.hpp"

/**
 * @file iwr6843_controller.cpp
 * @brief Quellcode für den IWR6843 Radar Controller.
 * @details Weitere Informationen stehen in der Klassenbeschreibung von @c Iwr6843Controller.
 */

using namespace std::chrono;


// -------------------------------------------------------------
// Konstruktor: Setzt die Ports und den Pfad zur Konfigurationsdatei
// -------------------------------------------------------------
Iwr6843Controller::Iwr6843Controller(
    const std::string& cfg_port,
    const std::string& data_port,
    const std::string& cfg_file)
  : uart_port_cfg(cfg_port),
    uart_port_data(data_port),
    cfg_file_path_(cfg_file)
{}


// -------------------------------------------------------------
// Destruktor (aktuell leer) – wichtig: hier später Thread stoppen!
// -------------------------------------------------------------
Iwr6843Controller::~Iwr6843Controller() {
}


// -------------------------------------------------------------
// Initialisiert den Sensor und startet den Polling-Thread
// -------------------------------------------------------------
bool Iwr6843Controller::initialize_and_capture_Data() {

    // Ausgabe der verwendeten UART-Ports
    std::cout << "IWR6843 UART ports set to: cfg=" << uart_port_cfg
              << ", data=" << uart_port_data << "\n";

    // Sensorobjekt erstellen
    sensor_ptr = std::make_unique<IWR6843>();

    // Sensor initialisieren
    int rc = sensor_ptr->init(uart_port_cfg, uart_port_data, cfg_file_path_);
    // (rc wird aktuell nicht geprüft – könnte man später machen)

    std::cout << "IWR6843 initialized successfully, starting poll thread\n";
    std::cout << "Sensor pointer is "
              << (sensor_ptr ? "valid" : "null")
              << std::endl;

    // Steuerflag setzen → Polling-Schleife soll laufen
    running = true;

    // Separaten Thread starten, der dauerhaft Radar-Daten abfragt
    polling_thread_ = std::thread(&Iwr6843Controller::pollingLoop, this);

    return true;
}


// -------------------------------------------------------------
// Polling-Thread: Liest permanent neue Radarframes ein
// -------------------------------------------------------------
void Iwr6843Controller::pollingLoop() {

    // Sicherheit: pointer prüfen
    if (!sensor_ptr) {
        std::cerr << "ERROR: sensor_ptr is null in pollingLoop()\n";
        return;
    }

    std::cout << "Sensor pointer is "
              << (sensor_ptr ? "valid" : "null")
              << std::endl;

    // Haupt-Loop: solange running == true bleibt, wird gepollt
    while (running) {

        // Neue Daten abfragen
        int newCount = sensor_ptr->poll();
        std::cout << "newCount "<< newCount;

        // Wenn keine neuen Daten → kurz schlafen und weitermachen
        if (newCount < 1) {
            std::this_thread::sleep_for(1ms);
            continue;
        }

        // Container für gelesene Frames
        std::vector<SensorData> frames;

        // Frames vom internen Puffer kopieren
        sensor_ptr->copyDecodedFramesFromTop(frames, newCount, true, 1000);

        // Für jedes Frame die enthaltenen Punktdaten extrahieren
        for (auto &f : frames) {

            std::vector<uint8_t> raw;

            try {
                raw = f.storedRawData;  // raw-Daten (falls benötigt)

                // TLV-Payload extrahieren (enthält die Punktwolke)
                std::vector<TLVPayloadData> payloads = f.getTLVPayloadData();

                // Durch alle Payloads iterieren
                for (const auto &pl : payloads) {

                    const auto &pts = pl.DetectedPoints_str;

                    // Wenn keine Punkte → nächsten Payload
                    if (pts.empty())
                        continue;

                    // Alle Radar-Punkte ausgeben
                    for (auto &p : pts) {
                        std::cout << "x: " << p.x_f
                                  << ",   y: " << p.y_f
                                  << ",   z: " << p.z_f
                                  << ",   doppler: " << p.doppler_f
                                  << std::endl;
                    }
                }
            }
            catch (const std::invalid_argument &e) {
                // Wenn die Payload nicht korrekt war → Frame überspringen
                std::cout << "FrameSkipped";
                continue;
            }
        } // Ende Frame-Schleife
    } // Ende while(running)
}
