# Rocket Phone
Stick this in the nose cone or e-bay of your high-power rocket...When it lands, it'll text you the maximum altitude and a link to <a href="https://www.google.com/maps">Google Maps</a> with your model's location. No expensive telemetry or tracking hardware, FCC Amateur Radio License or any additional equipment is required. *As long as you have cell service at your launch site and a smart phone you're good to go*. Just tap the link to open Google Maps and walk towards the point on the map to collect your model. You can build one for about $100 USD (including battery). You could probably make it cheaper by using a different Arduino, battery and/or pressure sensor.

## Demo ##

When the device starts (by connecting the battery) it'll verify there's a cell signal, aquire a GPS lock and then determine the launch site altitude (via a pressure sensor). After these checks are complete you will recieve a "Ready" SMS message on your phone with the launch site altitude and location. At this point the rocket can be flown. Once the software determines the rocket has landed it'll send another SMS message to your phone with the maximum altitude reached during flight and a link that will open Google Maps with the rocket's location displayed. You can use this to find and retrieve your model. Additional texts with updated location information will be sent every few minutes up to a configured maximum. To launch again, simply reset the device. 

*This is a screenshot from my desk, but I will get a live shot at the next launch I attend along with a live map view.*

<img src="https://github.com/robderstadt/RocketPhone/blob/master/images/sms.png"/>


## Build ##

There are only a few parts required:

1. <a href="https://www.adafruit.com/product/2542">Adafruit FONA 808 - Mini Cellular GSM + GPS Breakout</a>

2. <a href="https://www.sparkfun.com/products/11084?_ga=1.61646572.1700220216.1438891900">SparkFun Altitude/Pressure Sensor Breakout - MPL3115A2</a>

3. <a href="https://www.adafruit.com/products/1086">Arduino Micro with Headers - 5V 16MHz</a> (although any Arduino should work)

4. <a href="https://www.adafruit.com/products/258">Lithium Ion Polymer Battery - 3.7v 1200mAh</a>

5. <a href="https://www.sparkfun.com/products/8377">330Ω resistors</a> (2 required if using a 5V Arduino)

6. <a href="https://www.adafruit.com/products/2505">GSM 2G SIM Card</a>

You can direct solder all connections using the wiring diagram (below) or modify to suit your needs. In my case I just followed the hook-up guides provided for the parts so they could talk to my Arduino. I would suggest wiring up the FONO, ensure it works (use the <a href="https://learn.adafruit.com/adafruit-fona-808-cellular-plus-gps-breakout/arduino-test">Adafruit demo program</a>), then add and test the pressure sensor (using the <a href="https://learn.sparkfun.com/tutorials/mpl3115a2-pressure-sensor-hookup-guide?_ga=1.142074610.1700220216.1438891900">Sparkfun Demo</a>). You'll need to activate a SIM card for the FONA as well. One each component is working you can load the RocketPhone.ino onto your Arduio. This requires the Adafuit and Sparkfun libraries, which you should already have installed if you've been testing during your build. You should take a look at the RocketPhone.ino file and make sure to add your cell number where specified and ensure all the other parameters match your build. The default configuration will send five (5) SMS messages with a two minute delay between each message once the rocket has landed. A landing is determined when the rocket ascends 800ft above the launch site altitude and then descends below 800ft. There is a one minute delay between the time the rocket descends to 800ft and the first SMS message is sent.


<img src="https://github.com/robderstadt/RocketPhone/blob/master/images/RocketPhoneFritzing.png"/>


## Prototype ##

Here is the workgin prototype I built:

<img src="https://github.com/robderstadt/RocketPhone/blob/master/images/prototype.png"/>

I would recomend mounting inside a nose cone or e-bay. I'm fond of the mounting systems provided by <a href="http://www.ape-rc.com/#!/Rocketry/c/11909535/inview=product49409110&offset=0&sort=normal"> Attebery Performance Engineering</a>. 

## How to Use ##
Plug in the battery to the FONA (it'll power the Arduino), then press-and-hold the "key" button for 2 seconds. The FONA will startup. Next press and release the reset button on your Arduino. The software will initialize the FONA. You will recieve a text message once the system is ready for launch. At this point you can fly your model.

## Caveat Emptor ##

Use this information and source code at your own risk! I cannot be held liable or responsible for how you use this information. Be aware that you could loose your rocket (or worse) if something goes wrong.
