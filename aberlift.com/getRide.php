<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php
	/* get url parameters and set the fields right */
	$From = htmlspecialchars(utf8_encode($_GET["From"]));
	$To = htmlspecialchars(utf8_encode($_GET["To"]));
	$Date = htmlspecialchars(utf8_encode($_GET["Date"]));
	$Time = htmlspecialchars(utf8_encode($_GET["Time"]));
?>
<meta charset="UTF-8">
<html>
<head>
	<link href="Resources/calender/calender.css" rel="stylesheet" type="text/css">
	<link href="Resources/css/home.css" rel="stylesheet" type="text/css">
	<script src="Resources/JS/home.js?s"></script>
	<link href="Resources/css/spinner.css" rel="stylesheet" type="text/css">
<style>
.everything-container
{
	width:800px;
	margin-top:65px;
	padding-top:50px;
	padding-bottom:50px;
	margin-left:auto;
	margin-right:auto;
}
.clearfix:after { 
   content: " ";
   display: block; 
   height: 0;
   clear: both;
}
.headline
{
	color: rgba(0,0,0,0.54);
	font-size:2.45em;
	line-height:3em;
}
.create-ride-form
{
	float:left;
	/*background-color:red;*/
	width:400px;
	margin-right:50px;
	height:500px;
}
.verticalFormSpecs
{
	margin-top:25px;
	margin-right:0px;
	margin-left:0px;
	margin-bottom:0px;
	width:400px;
}
.create-ride-card
{
	float:left;
	width:300px;
	height:304px;
	padding:25px;
	box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
	font-size:14px;
	color:rgba(0,0,0,0.87);
	font-family: Roboto;
}
.ride-card-locations
{
	position:relative;
	height:200px;
	width:300px;
	background: #ffffff url('Resources/images/path_small.png') no-repeat center;
}
.ride-line-container
{
	margin-top:16px;
	width:300px;
	height:36px;
}
.confirmationDetailsMark
{
	width:252px;
	padding-left:16px;
	float:left;
}
.Head-graphic-location
{
	overflow:hidden;
	height:20px;
	line-height:20px;
	font-size:16px;
	color:rgba(0,0,0,0.87);
	-webkit-font-smoothing: antialiased;	
}
.Sub-graphic-location
{
	height:16px;
	line-height:16px;
	font-size: 11px;
	text-transform: uppercase;
	color:rgba(0,0,0,0.54);
	-webkit-font-smoothing: antialiased;
}
.left-location
{
	position: absolute;
	top:9px;
	left:36px;
}
.right-location
{
	text-align: right;
	position:absolute;
	top:144px;
	right:45px;
}
</style>
<script>
/* Fix Locations */
function fixDestination()
{
	var boardingPoint = document.getElementById('boardingPoint').value;
	var op = document.getElementById("destination").getElementsByTagName("option");
	for (var i = 0; i < op.length; i++)
	{
  		if(op[i].value == boardingPoint) 
  		{
  			op[i].disabled = true;
  		}
  		else
  		{
  			op[i].disabled = false;
  		}
	}
}
function fixBoarding()
{
	var destination = document.getElementById('destination').value;
	var op = document.getElementById("boardingPoint").getElementsByTagName("option");
	for (var i = 0; i < op.length; i++)
	{
  		if(op[i].value == destination) 
  		{
  			op[i].disabled = true;
  		}
  		else
  		{
  			op[i].disabled = false;
  		}
	}
}

/* Check that fields not blank */

function requestRide()
{
	document.getElementById('boardingPoint-error').style.opacity = "0"; 
	document.getElementById('destination-error').style.opacity = "0"; 
	document.getElementById('hbdsdf-error').style.opacity = "0"; 
	document.getElementById('time-error').style.opacity = "0"; 

	var boardingPoint = document.getElementById('boardingPoint').value;
	var destination = document.getElementById('destination').value;
	var boardingDate = document.getElementById('hbdsdf').value;
	var boardingTime = document.getElementById('time').value;

	var flag = 0;

	if (boardingPoint == "")
	{
		document.getElementById('boardingPoint-error').style.opacity = "1";
		flag++;
	}
	if (destination == "")
	{
		document.getElementById('destination-error').style.opacity = "1"; 
		flag++;
	}
	if (boardingDate.localeCompare("Date") == 1)
	{
		document.getElementById('hbdsdf-error').style.opacity = "1";
		flag++;
	}
	if (boardingDate == "Date")
	{
		document.getElementById('hbdsdf-error').style.opacity = "1";
		flag++;
	}
	if (boardingTime == "")
	{
		document.getElementById('time-error').style.opacity = "1";
		flag++;
	}

	if(flag == 0)
	{
		/* All fields correct */
		/* offer logic */

		document.getElementById("button-container").innerHTML = "<svg class='center spinner' style='margin-top:50px' width='32px' height='32px' viewBox='0 0 66 66' xmlns='http://www.w3.org/2000/svg'><circle class='path' fill='none' stroke-width='6' stroke-linecap='round' cx='33' cy='33' r='30'></circle></svg>";

		xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function () {
			if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
				var Reply = xmlhttp.responseText;
				if (Reply == "success")
				{
					// Success
					document.getElementById("message-window").innerHTML = '<img src="Resources/svg/check.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;"><div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px"><div style="text-align:center;font-size:18px;line-height:32px"class="accent">Request posted</div><br>All good! Your request for the lift has been posted. We\'ll text you immediately when someone offers you the lift. Stay tuned...</div><div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
					document.getElementById("messageWindow-full").style.display = "block";

					setTimeout(function(){
						document.getElementById("messageWindow-full").style.opacity = "1";
						document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
						document.getElementById("button-container").innerHTML='<div onclick="requestRide()" class="center header-button" style="display:inline-block;margin-top:50px">Publish request</div>';
					}, 250);
				}
				else if (Reply == "error2")
				{
					//alert("Looks like you've entered some wrong details. Please go back and try again.");
					document.getElementById("message-window").innerHTML = '<img src="Resources/svg/error.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;"><div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px"><div style="text-align:center;font-size:18px;line-height:32px"class="accent">Ouch! An error occured</div><br>There seems to be an error in the data we\'ve received from you. Please refresh this page and try again.</div><div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
					document.getElementById("messageWindow-full").style.display = "block";

					setTimeout(function(){
						document.getElementById("messageWindow-full").style.opacity = "1";
						document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
						document.getElementById("button-container").innerHTML='<div onclick="requestRide()" class="center header-button" style="display:inline-block;margin-top:50px">Publish request</div>';
					}, 250);
				}
				else
				{
					//unknown error
					document.getElementById("message-window").innerHTML = '<img src="Resources/svg/error.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;"><div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px"><div style="text-align:center;font-size:18px;line-height:32px"class="accent">Well, that\'s embarrasing</div><br>There was an unknown error while trying to post your lift. Please try posting the lift again. If nothing works, sign out and sign in again.</div><div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
					document.getElementById("messageWindow-full").style.display = "block";

					setTimeout(function(){
						document.getElementById("messageWindow-full").style.opacity = "1";
						document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
						document.getElementById("button-container").innerHTML='<div onclick="requestRide()" class="center header-button" style="display:inline-block;margin-top:50px">Publish request</div>';
					}, 250);
				}
			}
		}
		xmlhttp.open("POST","requestRide.php",true);
		xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
		xmlhttp.send("from=" + boardingPoint + "&to=" + destination + "&date=" + boardingDate + "&time=" + boardingTime);
	}
}

function closeMessage()
{
	document.getElementById("message-window").style.boxShadow = "0px 0px 0px rgb(0,0,0)";
	document.getElementById("messageWindow-full").style.opacity = "0";
	setTimeout(function(){ 	document.getElementById("message-window").innerHTML = ""; document.getElementById("messageWindow-full").style.display = "none"; }, 500);
}

function loadDate()
{
	var paramDate = getParameterByName("Date");
	if(!(typeof paramDate === 'undefined'))
	{
		document.getElementById("hbdsdf").value = paramDate;
	}
}

/* Generic Function to retrieve GET params */
function getParameterByName(name) {
    name = name.replace(/[\[]/, "\\[").replace(/[\]]/, "\\]");
    var regex = new RegExp("[\\?&]" + name + "=([^&#]*)"),
        results = regex.exec(location.search);
    return results === null ? "" : decodeURIComponent(results[1].replace(/\+/g, " "));
}
</script>
<style> /* Message Window Styles */
#messageWindow-full
{
	position:fixed;
	width:100%;
	height:100%;
	left:0;
	right:0;
	top:0;
	bottom:0;

	opacity:0;
	display:none;

	transition:0.25s ease-out;
	transition-delay:0.25s;

	background-color:rgba(0,0,0,0.1);
	z-index:1;
}
#message-Window
{
	height:450px;
	width:800px;
	margin-top:150px;
	margin-left:auto;
	margin-right:auto;
	background-color:white;
	transition:0.25s ease-out;
	transition-delay:0.25s;
}
</style>
</head>
<body onload="loadDate(); refreshGraph()">
	<?php include 'header.php';?>

	<div id="messageWindow-full">
		<div id="message-window">
		</div>
	</div>
	
	<div onclick="refreshGraph()" class="everything-container clearfix">
		<div class="headline">Publish a request</div>
		<div class="create-ride-form">
			<div class="center sub-sign-content">
				<select onblur="fixDestination(); refreshGraph()" id="boardingPoint" class="select verticalFormSpecs object" style="margin-top:0;">
					<?php
						/* If no GET header */
						if ($From == "" || is_null($From))
						{
							echo '<option value="" disabled="" selected="">Boarding Point</option>';
						}
					
						include 'DB_Connect.php';
						$Query = "SELECT * FROM AberLift_STATIONS ORDER BY Station_Name";
						$result = mysqli_query($con, $Query);
						if (!empty($result))
						{
							$headerMatchFound = "No";
							while($row = mysqli_fetch_array($result))
							{	
								$stationCode = $row['Station_Code'];
								$stationName = $row['Station_Name'];

								if ($From == "$stationCode")
								{								
									echo "<option value=\"";
									echo $stationCode;
									echo "\" selected>";
									echo $stationName;
									echo "</option>";
									$headerMatchFound = "Yes";
								}
								else
								{
									echo "<option value=\"";
									echo $stationCode;
									echo "\">";
									echo $stationName;
									echo "</option>";
								}
							}
							if ($headerMatchFound == "No")
							{
								echo '<option value="" disabled="" selected="">Boarding Point</option>';
							}
						}
					?>
				</select>
				<div id="boardingPoint-error" class="error">Please select the boarding point</div>

				<select onblur="fixBoarding(); refreshGraph()" id="destination" class="select verticalFormSpecs object">
					<?php
						/* If no GET header */
						if ($To == "" || is_null($To))
						{
							echo '<option value="" disabled="" selected="">Destination</option>';
						}
					
						$Query = "SELECT * FROM AberLift_STATIONS ORDER BY Station_Name";
						$result = mysqli_query($con, $Query);
						if (!empty($result))
						{
							$headerMatchFound = "No";
							while($row = mysqli_fetch_array($result))
							{	
								$stationCode = $row['Station_Code'];
								$stationName = $row['Station_Name'];

								if ($To == "$stationCode")
								{								
									echo "<option value=\"";
									echo $stationCode;
									echo "\" selected>";
									echo $stationName;
									echo "</option>";
									$headerMatchFound = "Yes";
								}
								else
								{
									echo "<option value=\"";
									echo $stationCode;
									echo "\">";
									echo $stationName;
									echo "</option>";
								}
							}
							if ($headerMatchFound == "No")
							{
								echo '<option value="" disabled="" selected="">Destination</option>';
							}
						}
					?>
				</select>
				<div id="destination-error" class="error">Please select the destination</div>

				<input onblur="refreshGraph()" class="object verticalFormSpecs" type="text" id="hbdsdf" placeholder="Date"></input>
				<div id="hbdsdf-error" class="error">Please select the date</div>

				<select onblur="refreshGraph()" id="time" class="select verticalFormSpecs object">
					<?php

						if ($Time == "" || is_null($To))
						{
							echo '<option value="" disabled="" selected="">Time</option>';
							$isTimeSet = "Yes";
						}

						$isTimeSet = "No";
						/* code to echo times */
						for ($i=0; $i<24; $i++)
						{
							for ($j=0; $j<4; $j++)
							{
								($i<10)?($hour='0'.$i):($hour=$i);
								($j==0)?($minute='00'):($minute=$j*15);
								if ($Time == $hour . '' . $minute)
								{
									echo '<option value="' . $hour . '' . $minute . '" selected>' . $hour . ':' . $minute . '</option>';
									$isTimeSet = "Yes";
								}
								else
								{
									echo '<option value="' . $hour . '' . $minute . '">' . $hour . ':' . $minute . '</option>';
								}
							}
						}

						if ($isTimeSet == "No")
						{
							echo '<option value="" disabled="" selected="">Time</option>';
						}
					?>
				</select>
				<div id="time-error" class="error">Please select the time</div>

				<div id="button-container" style="text-align:center;">
					<div onclick="requestRide()" class="center header-button" style="display:inline-block;margin-top:50px">Publish request</div>
				</div>
			</div>
		</div>
		<div class="create-ride-card">
			<div class="ride-card-locations">
				<div class="confirmationDetailsMark left-location">
					<div id="gBoardingPoint" class="Head-graphic-location"></div>
					<div class="Sub-graphic-location">Boarding Point</div>
				</div>
				<div class="confirmationDetailsMark right-location">
					<div id="gDestination" class="Head-graphic-location"></div>
					<div class="Sub-graphic-location">Destination</div>
				</div>
			</div>
			<div class="ride-line-container">
				<div style="float:left;height:32px;width:32px;background:url('Resources/svg/calender.svg');background-repeat:no-repeat;background-size:32px;background-position:0px 0px;"></div>
				<div class="confirmationDetailsMark">
					<div id="gDate" class="Head-graphic-location">Date</div>
					<div class="Sub-graphic-location">Date</div>
				</div>
			</div>
			<div class="ride-line-container">
				<div style="float:left;height:32px;width:32px;background:url('Resources/svg/time.svg');background-repeat:no-repeat;background-size:32px;background-position:0px 0px;"></div>
				<div class="confirmationDetailsMark">
					<div id="gTime" class="Head-graphic-location"></div>
					<div class="Sub-graphic-location">Time</div>
				</div>
			</div>
		</div>
	</div>
	<?php include 'footer.php';?>
	<script src='Resources/JS/jquery.min.js'></script> 
	<script src="Resources/calender/ios-7-date-picker.js"></script>
	<script>
		$(document).on('change','#boardingPoint',function(){ refreshGraph(); });
		$(document).on('change','#destination',function(){ refreshGraph(); });
		$(document).on('change','#hbdsdf',function(){ refreshGraph(); });
		$(document).on('change','#time',function(){ refreshGraph(); });

		function refreshGraph()
		{	
			document.getElementById("gBoardingPoint").innerHTML = $( "#boardingPoint option:selected" ).text();
			document.getElementById("gDestination").innerHTML = $( "#destination option:selected" ).text();
			document.getElementById("gDate").innerHTML = $( "#hbdsdf" ).val();
			if (document.getElementById("gDate").innerHTML == "")
			{
				document.getElementById("gDate").innerHTML = "Date";	
			}
			document.getElementById("gTime").innerHTML = $( "#time option:selected" ).text();
		}

		setInterval(function(){ refreshGraph() }, 100);
	</script>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
</body>
</html>