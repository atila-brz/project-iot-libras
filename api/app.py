import paho.mqtt.client as mqtt
from flask import Flask, request, jsonify
from flask_cors import CORS

MQTT_BROKER = "broker.hivemq.com"
MQTT_PORT = 1883
MQTT_TOPIC = "esp32/display/letra" 

app = Flask(__name__)
CORS(app)

def on_connect(client, userdata, flags, rc):
    if rc == 0:
        print(f"Conectado ao Broker MQTT: {MQTT_BROKER}")
    else:
        print(f"Falha na conexão MQTT. Código: {rc}")

client = mqtt.Client()
client.on_connect = on_connect

try:
    client.connect(MQTT_BROKER, MQTT_PORT, 60)
    client.loop_start()
except Exception as e:
    print(f"Erro ao conectar no MQTT: {e}")

@app.route("/api/enviar-letra", methods=["POST"])
def enviar_letra():
    data = request.json
    print(f"Recebido: {data}")

    if not data or 'letra' not in data:
        return jsonify({"erro": "O campo 'letra' é obrigatório"}), 400

    letra_recebida = str(data['letra'])

    if len(letra_recebida) != 1:
        return jsonify({"erro": "Envie apenas UM caractere."}), 400

    try:
        client.publish(MQTT_TOPIC, letra_recebida)
        print(f"Publicado no MQTT [{MQTT_TOPIC}]: {letra_recebida}")
        
        return jsonify({
            "status": "sucesso", 
            "mensagem": f"Letra '{letra_recebida}' enviada para o display."
        })

    except Exception as e:
        print(f"Erro ao publicar: {e}")
        return jsonify({"erro": "Falha ao comunicar com MQTT"}), 500

if __name__ == "__main__":
    print("Iniciando API de Controle do Display...")
    app.run(host='0.0.0.0', port=5000, debug=True)