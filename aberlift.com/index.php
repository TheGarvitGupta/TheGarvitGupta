<?php
	session_start();
?>
<meta id="viewport" name="viewport" content="width=1170">
<meta charset="UTF-8">
<html>
<head>
	<link href="Resources/calender/calender.css" rel="stylesheet" type="text/css">
	<link href="Resources/css/home.css" rel="stylesheet" type="text/css">
	<script src='Resources/JS/jquery.min.js'></script> 
	<script src="Resources/JS/home.js"></script>

	<link rel="prefetch" href="Resources/images/lede-all/lede_2.jpeg">
	<link rel="prefetch" href="Resources/images/lede-all/lede_3.jpeg">
	<link rel="prefetch" href="Resources/images/lede-all/lede_4.jpeg">
</head>
<body>
	<?php include 'header.php'; ?>
	<div id="lede-container">
		<div class="social-buttons">			
			<!--<div class="social-image icon-pinterest"></div>-->
			<!--<div class="social-image icon-linkedin"></div>-->
			<!--<div class="social-image icon-google"></div>-->
			<div class="social-image icon-twitter"></div>
			<div class="social-image icon-facebook"></div>
		</div>
	</div>
	<div>
		<div class="toggler">
			<div class="toggle-a anim" id="sign-up-button"  onclick="showOffer()">
				Offer a lift
			</div>
			<div class="toggle-b anim" id="sign-in-button" onclick="showGet()">
				Get a lift
			</div>
		</div>
	</div>
	<div class="search-rides">
		<table style="width:100%;height:100%" id="outerDiv" cellpadding="0" cellspacing="0" border="0">
		    <tr>
		        <td valign="middle" align="center" id="innerDiv">
					<!-- Everything here will be centered -->
					<div id="searchContainer" class="searchForm">
						<table style="height:100%" id="outerDiv" cellpadding="0" cellspacing="0" border="0">
		    				<tr>
	        					<td valign="middle" align="center" id="innerDiv">
									<select onblur="fixDestination()" id="get-board" class="select object">
										<option value="" disabled="" selected="">Boarding Point</option>
										<?php
											include 'DB_Connect.php';
											$Query = "SELECT * FROM AberLift_STATIONS ORDER BY Station_Name";
											$result = mysqli_query($con, $Query);
											if (!empty($result))
											{
												while($row = mysqli_fetch_array($result))
												{	
													$stationCode = $row['Station_Code'];
													$stationName = $row['Station_Name'];

													echo "<option value=\"";
													echo $stationCode;
													echo "\">";
													echo $stationName;
													echo "</option>";
												}
											}
										?>
									</select>
									<div id="get-board-error" class="error">Please select a location</div> 
								</td>
	        					<td valign="middle" align="center" id="innerDiv">
									<select onblur="fixBoarding()" id="get-destination" class="select object">
										<option value="" disabled="" selected="">Destination</option>
										<?php
											$result = mysqli_query($con, $Query);
											if (!empty($result))
											{
												while($row = mysqli_fetch_array($result))
												{	
													$stationCode = $row['Station_Code'];
													$stationName = $row['Station_Name'];

													echo "<option value=\"";
													echo $stationCode;
													echo "\">";
													echo $stationName;
													echo "</option>";
												}
											}
										?>
									</select>
									<div id="get-destination-error" class="error">Please select a location</div> 
								</td>
	        					<td valign="middle" align="center" id="innerDiv">
									<input class="object" type="input" id="hbdsdf" placeholder="Date"></input>
									<div id="get-hbdsdf-error" class="error">Please select the date</div>
								</td>	
	        					<td valign="middle" align="center" id="innerDiv">
									<select id="get-time" class="select object">
										<option value="" disabled="" selected="">Time</option>
										<option value="0000">00:00</option>
										<option value="0015">00:15</option>
										<option value="0030">00:30</option>
										<option value="0045">00:45</option>
										<option value="0100">01:00</option>
										<option value="0115">01:15</option>
										<option value="0130">01:30</option>
										<option value="0145">01:45</option>
										<option value="0200">02:00</option>
										<option value="0215">02:15</option>
										<option value="0230">02:30</option>
										<option value="0245">02:45</option>
										<option value="0300">03:00</option>
										<option value="0315">03:15</option>
										<option value="0330">03:30</option>
										<option value="0345">03:45</option>
										<option value="0400">04:00</option>
										<option value="0415">04:15</option>
										<option value="0430">04:30</option>
										<option value="0445">04:45</option>
										<option value="0500">05:00</option>
										<option value="0515">05:15</option>
										<option value="0530">05:30</option>
										<option value="0545">05:45</option>
										<option value="0600">06:00</option>
										<option value="0615">06:15</option>
										<option value="0630">06:30</option>
										<option value="0645">06:45</option>
										<option value="0700">07:00</option>
										<option value="0715">07:15</option>
										<option value="0730">07:30</option>
										<option value="0745">07:45</option>
										<option value="0800">08:00</option>
										<option value="0815">08:15</option>
										<option value="0830">08:30</option>
										<option value="0845">08:45</option>
										<option value="0900">09:00</option>
										<option value="0915">09:15</option>
										<option value="0930">09:30</option>
										<option value="0945">09:45</option>
										<option value="1000">10:00</option>
										<option value="1015">10:15</option>
										<option value="1030">10:30</option>
										<option value="1045">10:45</option>
										<option value="1100">11:00</option>
										<option value="1115">11:15</option>
										<option value="1130">11:30</option>
										<option value="1145">11:45</option>
										<option value="1200">12:00</option>
										<option value="1215">12:15</option>
										<option value="1230">12:30</option>
										<option value="1245">12:45</option>
										<option value="1300">13:00</option>
										<option value="1315">13:15</option>
										<option value="1330">13:30</option>
										<option value="1345">13:45</option>
										<option value="1400">14:00</option>
										<option value="1415">14:15</option>
										<option value="1430">14:30</option>
										<option value="1445">14:45</option>
										<option value="1500">15:00</option>
										<option value="1515">15:15</option>
										<option value="1530">15:30</option>
										<option value="1545">15:45</option>
										<option value="1600">16:00</option>
										<option value="1615">16:15</option>
										<option value="1630">16:30</option>
										<option value="1645">16:45</option>
										<option value="1700">17:00</option>
										<option value="1715">17:15</option>
										<option value="1730">17:30</option>
										<option value="1745">17:45</option>
										<option value="1800">18:00</option>
										<option value="1815">18:15</option>
										<option value="1830">18:30</option>
										<option value="1845">18:45</option>
										<option value="1900">19:00</option>
										<option value="1915">19:15</option>
										<option value="1930">19:30</option>
										<option value="1945">19:45</option>
										<option value="2000">20:00</option>
										<option value="2015">20:15</option>
										<option value="2030">20:30</option>
										<option value="2045">20:45</option>
										<option value="2100">21:00</option>
										<option value="2115">21:15</option>
										<option value="2130">21:30</option>
										<option value="2145">21:45</option>
										<option value="2100">22:00</option>
										<option value="2215">22:15</option>
										<option value="2230">22:30</option>
										<option value="2245">22:45</option>
										<option value="2300">23:00</option>
										<option value="2315">23:15</option>
										<option value="2330">23:30</option>
										<option value="2345">23:45</option>										
									</select>
									<div id="get-time-error" class="error">Please select the time</div> 									
								</td>	
	        					<td valign="middle" align="center" id="innerDiv">
									<div id="form-submit" class="button-leaf" onclick="searchRides()"></div>
								</td>				    
						</table>
					</div>
					<!-- Until here -->
		        </td>
		    </tr>
		</table>
	</div>
	<div class="features-container">
		<div class="features-inner">
			<div class="features-divider"></div>
			<table style="width:100%;" id="outerDiv" cellpadding="0" cellspacing="0" border="0">
			    <tr>
			        <td style="width:50%">
			        	<div class="frame-icon hideme" style="background-image: url('Resources/svg/timer.svg');"></div>
			        	<div class="frame-text-left">
			        		<div class="heading-frame-text">TIME AND COST</div>
							<div class="description-frame-text">
			        			<div>By using AberLift you'll save £1,620 per academic year just by travelling in Aberystwyth.</div>
			        			<div style="padding-top:10px;">Well, that is a lot of beer money!</div>
			        		</div>
			        	</div>
			        </td>
			        <td style="width:50%">
			        	<div class="frame-icon hideme" style="margin-left:85px;background-image: url('Resources/svg/group.svg');"></div>
			        	<div class="frame-text-right">
			        		<div class="heading-frame-text">SOCIAL</div>
			        		<div class="description-frame-text">
			        			<div>AberLift would greatly enhance your student experience, by getting you places faster and cheaper.</div>
			        			<div style="padding-top:10px;">All along while meeting your friends and making new ones!</div>
			        		</div>
			        	</div>
			        </td>
			    </tr>
			    <tr class="inter-spacing">
			        <td style="width:50%">
			        	<div class="frame-icon hideme" style="background-image: url('Resources/svg/security.svg');"></div>
			        	<div class="frame-text-left">
			        		<div class="heading-frame-text">SECURE</div>
			        		<div class="description-frame-text">
			        			<div>The platform is exclusive to students with an Aberystwyth University email address. And our servers store data about each trip that occurs.</div>
			        			<div style="padding-top:10px;">Feel safe jumping in the car with Jimmy from Block 4!</div>
			        		</div>
			        	</div>
			        </td>
			        <td style="width:50%">
			        	<div class="frame-icon hideme" style="margin-left:85px;background-image: url('Resources/svg/check_white.svg');"></div>
			        	<div class="frame-text-right">
			        		<div class="heading-frame-text">ECO-FRIENDLY</div>
			        		<div class="description-frame-text">
			        			<div>Travel knowing that you are helping to minimise the carbon footprint of Aberystwyth.</div>
			        			<div style="padding-top:10px;">By using AberLift, you ensure that there are less cars on the road. This means less traffic too!</div>
			        		</div>
			        	</div>
			        </td>
			    </tr>		    
			</table>
		</div>
	</div>
	<?php include 'footer.php' ?>
	<script src="Resources/calender/ios-7-date-picker.js"></script>
	<script type="text/javascript">
	  	$(window).load(function() {           
		  	var i =0; 
		  	var images = ['Resources/images/lede-all/lede_2.jpeg','Resources/images/lede-all/lede_3.jpeg','Resources/images/lede-all/lede_4.jpeg','Resources/images/lede-all/lede_1.jpeg'];
		  	var image = $('#lede-container');
			setInterval(function(){
				image.css('background-image', 'url(' + images [i++] +')');
				if(i == images.length)
				{
					i = 0;
				}
		  	}, 7500);            
	 	});
	</script>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
	<script>
		/* prefetch images */
		Image1= new Image(1280,854);
		Image1.src = "Resources/images/lede-all/lede_2.jpeg";

		Image2= new Image(1280,854);
		Image2.src = "Resources/images/lede-all/lede_3.jpeg";

		Image3= new Image(1280,854);
		Image3.src = "Resources/images/lede-all/lede_4.jpeg";
	</script>
</body>
</html>