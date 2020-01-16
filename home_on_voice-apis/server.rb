require 'bundler'
require './lib/motor'
require './lib/light'
require './lib/mqtt_client'

MqttClient._load
Bundler.require
post '/' do
  Ralyxa::Skill.handle(request)
end
