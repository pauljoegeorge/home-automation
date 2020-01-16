require 'mqtt'
require 'timeout'
class Motor
  def intialize
    # `stopped` | `running` | `error`
    @state = 'stopped'
  end

  # turn ON/OFF motor
  def self.on(status)
    message = status ? "running" : "stopped"
    client = MqttClient.instance.client
    # notify ESP32 to start "running"
    client.publish('/request', message)
    # wait for response from ESP32
    begin
      Timeout::timeout(2) do
        client.get('/response') do |topic, message|
          # ESP32 should return `running`/`stopped`
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
    @state == 'running'
  end
end
