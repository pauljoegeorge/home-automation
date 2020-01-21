require 'bundler'
require './lib/motor'
require './lib/light'
require './lib/tv'
require './lib/mqtt_client'

MqttClient._load
Bundler.require
post '/' do
  Ralyxa::Skill.handle(request)
end
