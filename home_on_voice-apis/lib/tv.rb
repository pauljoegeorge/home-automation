require 'mqtt'
require 'timeout'
class Tv
  def intialize
    # `off` | `on` | `error`
    @state = 'tv_off'
  end

  # turn ON/OFF bedroom light
  def self.on(status)
    message = status ? "tv_on" : "tv_off"
    client = MqttClient.instance.client
    # notify ESP32 to turn "on/off" the light
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
    @state == 'tv_on'
  end
end
