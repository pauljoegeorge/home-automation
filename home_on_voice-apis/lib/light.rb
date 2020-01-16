require 'mqtt'
require 'timeout'
class Light
  def intialize
    # `off` | `on` | `error`
    @state = 'off'
  end

  # turn ON/OFF motor
  def self.on(status)
    message = status ? "on" : "off"
    client = MqttClient.instance.client
    # notify ESP32 to start "running"
    client.publish('/request', message)
    # wait for response from ESP32
    begin
      Timeout::timeout(2) do
        client.get('/response') do |topic, message|
          # ESP32 should return `on`/`off`
          @state = message
          break
        end
      end
    rescue Timeout::Error
      @state = "error"
    end
  end

  def self.state
    return @state
  end

  def self.running?
    @state == 'on'
  end
end
