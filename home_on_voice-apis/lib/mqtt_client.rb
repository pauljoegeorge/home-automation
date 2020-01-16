require 'mqtt'
require 'singleton'

class MqttClient
  include Singleton
  attr_accessor :client
  def self._load
    conn_opts = {
        remote_host: ENV["MQTT_HOST"],
        remote_port: ENV["MQTT_PORT"],
        username: ENV["MQTT_USERNAME"],
        password: ENV["MQTT_PASSWORD"]
    }
    instance.client = MQTT::Client.connect(conn_opts)
  end
end