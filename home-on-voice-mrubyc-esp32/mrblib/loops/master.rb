SERVO_MAX_DEGREE = 90
initialise_wifi()
mqtt = Mqtt.new
motor = Motor.new(18, 50, 0, 0) #GPIO pin, frequency, duty_cycle_a, duty_cycle_b
light = Light.new
tv = Tv.new
mqtt_retry = 0
while true
  if connected_to_network
    if connected_to_mqqt_broker
      Remote.new
      mqtt_retry = 0
      if mqtt.new_message?
        state =  received_message
        # puts "============================="
        # puts "received new message"
        # puts "new: " + state
        # puts motor.previous_state
        # puts light.previous_state
        # puts "============================="
        if state != motor.previous_state || state != light.previous_state
          #mqtt.update_state(state)
          case state
          when "running"
            motor.update_state(state)
            for count in 0...SERVO_MAX_DEGREE
                angle = motor.calc_pulsewidth(count)
                run_servo(angle) # will run motor
                sleep(0.002) # Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
            end
          when "stopped"
            motor.update_state(state)
            sleep(1)
          when "on"
            light.update_state(state)
            send_signal(0x41B6D52A)
            sleep(1)
          when "off"
            light.update_state(state)
            send_signal(0x41B6D52A)
            sleep(1)
          when "tv_on"
            tv.update_state(state)
            send_signal(0x1A2EEC3B)
            sleep(1)
          when "tv_off"
            tv.update_state(state)
            send_signal(0x1A2EEC3B)
            sleep(1)
          end
        else
          sleep(1)
        end
      else
        sleep(1)
      end
      # if motor.has_to_run?
      #   for count in 0...SERVO_MAX_DEGREE
      #       angle = motor.calc_pulsewidth(count)
      #       run_servo(angle) # will run motor
      #       sleep(0.002) # Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
      #   end
      # elsif light.has_to_on?
      #   turn_light_on()
      #   sleep(1)
      # else
      #   sleep(1)
      # end
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
