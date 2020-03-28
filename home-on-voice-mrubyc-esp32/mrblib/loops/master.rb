SERVO_MAX_DEGREE = 90
initialise_wifi()
mqtt = Mqtt.new
motor = Motor.new(18, 50, 0, 0) #GPIO pin, frequency, duty_cycle_a, duty_cycle_b
light = Light.new
tv = Tv.new
Remote.new
mqtt_retry = 0
while true
  if connected_to_network
    if connected_to_mqqt_broker
      mqtt_retry = 0
      if mqtt.new_message?
        state =  received_message
        if state != motor.previous_state || state != light.previous_state || state != tv.previous_state
          case state
          when "running"
            motor.update_state(state)
            for count in 0...SERVO_MAX_DEGREE
                angle = motor.calc_pulsewidth(count)
                run_servo(angle) # will run motor
                sleep(0.002) # Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
            end
          when "on"
            light.update_state(state)
            send_signal(0) # Switch LIGHT ON
            sleep(1)
          when "off"
            light.update_state(state)
            send_signal(0)  # Switch LIGHT OFF
            sleep(1)
          when "tv_on"
            tv.update_state(state)
            send_signal(1)  # Switch TV ON
            sleep(1)
          when "tv_off"
            tv.update_state(state)
            send_signal(1)  # Switch TV OFF
            sleep(1)
          end
        else
          sleep(1)
        end
      else
        sleep(1)
      end
    else
      if mqtt_retry < 1
        mqtt_retry += 1
        mqtt_app_start()
      end
      sleep(2)
    end
  else
    puts "connecting to network"
    sleep(2)
  end
end
