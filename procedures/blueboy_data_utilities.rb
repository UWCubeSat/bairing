# Blueboy Data Screen Utilities: blueboy_data_utilities.rb
# Author: Sebastian S

#################################################################
# Define helpful subroutines useful by multiple test procedures
#################################################################

# Get the mode number from the given widget
def get_attitude_mode(text)
  case text
  when "Raw"
    return 0
  when "Euler"
    return 1
  when "Quaternion"
    return 2
  else
    return 0
  end
end

def begin_attitude(device)
  device = device.upcase
  period = get_named_widget("#{device}_PERIOD").text.to_i;
  mode = get_attitude_mode(get_named_widget("#{device}_MODE").text)
  
  if not period.between?(0, 65535)
    return;
  end

  case device
  when "OWN"
	id = 16
  when "TEST"
    id = 32
  else
    id = 0
  end
  
  cmd("BLUEBOY BEGIN#{device}ATT with ID #{id}, PERIOD #{period}, TYPE #{mode}")
end

def begin_attitude_all()
  begin_attitude("OWN")
  begin_attitude("TEST")
end

def end_attitude(device)
  device = device.upcase
  
  case device
  when "OWN"
	id = 17
  when "TEST"
    id = 33
  else
    id = 0
  end
  
  cmd("BLUEBOY END#{device}ATT with ID #{id}")
end

def end_attitude_all()
  end_attitude("OWN")
  end_attitude("TEST")
end