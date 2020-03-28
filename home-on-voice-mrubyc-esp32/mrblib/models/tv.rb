class Tv
  def initialize()
    # set_servo_gpio(pin)
    # set_servo_params(frequency, duty_cycle_a, duty_cycle_b)
    #init_rmt_config
    @status = "tv_off"
  end

  def has_to_on?
    if new_request
      send_notification(received_message) #notify sinatra side whenever a new message is received
      if received_message != @status
        if received_message == "tv_on"
          @status = "tv_on"
          return true
        end
        if received_message == "tv_off"
          @status = "tv_off"
          return false
        end
      end
    else
      if @status == "tv_on"
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
