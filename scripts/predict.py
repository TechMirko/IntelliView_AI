from ultralytics import YOLO
import os
import shutil
from paho.mqtt import client as mqtt_client

# A0:A3:B3:2A:56:DE <-- mac address esp32 cam

# Impostazioni MQTT
broker = 'broker.emqx.io'
port = 1883
topic = "tecnicamente/riconoscimento"
client_id = "IntelliView"

def connect_mqtt():
    def on_connect(client, userdata, flags, rc):
        if rc == 0:
            print("Connected to MQTT Broker!")
        else:
            print("Failed to connect, return code %d\n", rc)

    client = mqtt_client.Client(client_id)
    client.on_connect = on_connect
    client.connect(broker, port)
    return client

def publish(client, msg: str):
    result = client.publish(topic, msg)
    if result[0] == 0:
        print("Invio avvento con successo")
    else:
        print("Invio non riuscito")


# ------ FUNZIONE PER RICONOSCERE UNA IMMAGINE ------ #
def detect(i: str) -> None:
    model = YOLO('path\of\the\model')
    model.predict(i, conf=0.7, save=True, save_txt=True)

# ------ FUNZIONE PER LEGGERE IL FILE CON I RISULTATI ------ #
def get_detection_name() -> str:
    path = f"path\of\the\predicts\labels"
    permissions = 0o755

    # ottengo il file txt senza conoscerne il nome
    files = os.listdir(path)
    
    for file_name in files:
        if file_name.endswith(".txt"):
            path = os.path.join(path, file_name)

            # leggo il file
            with open(path, "r") as file:
                content = file.read()
            os.chmod(path, permissions)
            print(content)

            # gestione oggetto riconsociuto
            if content.split(" ")[0] == '0':
                print('Oggetto riconosciuto: 5 euro')
                return '5 euro'
            elif content.split(" ")[0] == '1':
                print("Oggetto riconosciuto: semaforo verde")
                return 'semaforo verde'
            elif content.split(" ")[0] == '2':
                print("Oggetto riconosciuto: semaforo rosso")
                return 'semaforo rosso'

# ------ FUNZIONE PER RIMUOVERE LA CARTELLA CREATA PER INIZIALIZZARE ------ #
def init_folder() -> None:
    path = "path\of\the\predicts\labels"
    shutil.rmtree(path)
    imgToErase = "path\of\the\imgs from the server\"
    os.remove(imgToErase)

# ------ FUNZIONE CHE MI DA I RISULTATI ELABORATI ------ #
def recognise() -> None:
    img = f"path\of\the\imgs from the server"
    # esegui il riconoscimento
    detect(img)
    oggetto = get_detection_name()
    client = connect_mqtt()
    client.loop_start()
    # Controllo se ho riconosciuto o non un oggetto
    if oggetto == None:
        oggetto = "nessun oggetto"
        publish(client, oggetto)
    else:
        publish(client, oggetto)
    client.loop_stop()
    init_folder()
    main() # chiamo main() per rendere infinito il tutto

def main() -> None:
    print("In attesa della foto...")
    path = "path\of\the\imgs from the server\"
    riconosciuto = 0
    while riconosciuto == 0:
        files = os.listdir(path)
        for file_name in files:
            if file_name.endswith(".jpg"):
                print("Presente")
                recognise()
                riconosciuto += 1
            else:
                continue

    
if __name__ == "__main__":
    main()
