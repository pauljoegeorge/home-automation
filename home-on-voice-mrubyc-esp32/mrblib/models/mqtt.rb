class Mqtt
  def new_message?
    if new_request
      # puts "NEW REQUEST"
      send_notification(received_message) #notify sinatra side whenever a new message is received
      return true
    else
      # puts "NO NEW REQUEST  " + @status
      return false
    end
  end
end
