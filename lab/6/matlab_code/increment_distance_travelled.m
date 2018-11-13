distanceTravelled = 0;
wheelCircumference = 6.4*pi/10000 % in km

for 1:inf
    distanceTravelled = distanceTravelled + (rpm*wheelCircumference);

end
