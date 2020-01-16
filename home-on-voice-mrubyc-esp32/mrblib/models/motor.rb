class Motor
  SERVO_MIN_PULSEWIDTH = 1000 #Minimum pulse width in microsecond
  SERVO_MAX_PULSEWIDTH = 2000 #Maximum pulse width in microsecond
  SERVO_MAX_DEGREE = 90 #Maximum angle in degree upto which servo can rotate

  def initialize(pin, frequency, duty_cycle_a, duty_cycle_b)
    set_servo_gpio(pin)
    set_servo_params(frequency, duty_cycle_a, duty_cycle_b)
    @status = "stopped"
  end

  def calc_pulsewidth(degree_of_rotation)
    pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)))
    return pulsewidth
  end

  def has_to_run?
    if new_request
      send_notification(received_message) #notify sinatra side whenever a new message is received
      if received_message != @status
        if received_message == "running"
          @status = "running"
          return true
        end
        if received_message == "stopped"
          @status = "stopped"
          return false
        end
      end
    else
      if @status == "running"
        return true
      end
      return false
    end
  end

  def previous_state
    return @status
  end

  def update_state(state)
    @status = state
  end
end
