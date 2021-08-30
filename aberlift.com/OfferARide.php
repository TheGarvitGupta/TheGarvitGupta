<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}

	/* get url parameters and set the fields right */
	$From = htmlspecialchars(utf8_encode($_GET["From"]));
	$To = htmlspecialchars(utf8_encode($_GET["To"]));
	$Date = htmlspecialchars(utf8_encode($_GET["Date"]));
	$Time = htmlspecialchars(utf8_encode($_GET["Time"]));

	include 'DB_Connect.php';
?>
<meta charset="UTF-8">
<html>
<head>
	<link href="Resources/calender/calender.css" rel="stylesheet" type="text/css">
	<link href="Resources/css/home.css" rel="stylesheet" type="text/css">
	<link rel="prefetch" href="Resources/svg/no_internet.svg">
	<link href="Resources/css/spinner.css" rel="stylesheet" type="text/css">
	<script src='Resources/JS/jquery.min.js'></script> 
	<script src="Resources/JS/home.js"></script> 
<style>
.everything-container
{
	width:1170px;
	margin-top:65px;
	padding-top:50px;
	padding-bottom:50px;
	margin-left:auto;
	margin-right:auto;

	transition:0.25s ease-out;
	transition-delay: 0.25s;
	opacity:1;
}
.filters-container
{
	float:left;
	height:430px;
	width:350px;
	margin-right:20px;
	box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
}
.clearfix:after { 
   content: " ";
   display: block; 
   height: 0; 
   clear: both;
}
.right-container
{
	float: left;
	background-color:white;
	width:800px;	
}
.notif-top
{
	text-transform: uppercase;
	font-size:16px;
	padding:16px;
	box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
	-webkit-transition: all 250ms;
	moz-transition: all 250ms;
	text-align: center;
	width:648px;
	margin-left:auto;
	margin-right:auto;
}
.accent
{
	color:#2196F3;
}
.bold
{
	font-weight:700;
}
.headline
{
	color: rgb(0,0,0);
	font-size:50px;
	font-family:Roboto;

	padding-top:25px;
	padding-bottom:15px;
}
.sub-line
{
	padding-bottom:50px;
	color:rgba(0,0,0,0.75);
	-webkit-font-smoothing: antialiased;
	font-family: Roboto;
}
.ride-container
{
	margin-top:1em;
	margin-left:auto;
	margin-right:auto;
	width:640px;
	height:160px;
	padding:20px;
	box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
	-webkit-transition: all 250ms;
	moz-transition: all 250ms;
}
.ride-container:hover
{
	cursor:pointer;
	box-shadow: 0 3px 6px 0 rgba(0, 0, 0, 0.16), 0 3px 6px 0 rgba(0, 0, 0, 0.23);
}
.ride-container:active
{
	box-shadow: 0 10px 20px 0 rgba(0, 0, 0, 0.19), 0 6px 6px 0 rgba(0, 0, 0, 0.23);
}
.ride-person-container
{
	float:left;
	width:150px;
	height:160px;
	padding-right: 16px;
	margin-right:23px;
	border-right: solid 1px rgba(0,0,0,0.12);
}
.ride-picture-container
{
	height:110px;
	width:110px;
	border-radius:100%;
	background-size: cover;
	background-position: center;
	margin-left:auto;
	margin-right:auto;
}
.ride-text-container
{
	width:100%;
	height:50px;
	line-height:50px;
	font-size:1em;
	font-family: Roboto;
	color:rgba(0,0,0,0.54);
	text-align:center;
	overflow:hidden;
	-webkit-font-smoothing:antialiased;
	font-weight: 500;
}
.ride-info-container
{
	float:left;
	width:440px;
	height:160px;
	margin-right:10px;
}
.mright
{
	margin-right: 20px;
}
.ride-info-section
{
	float:left;
	height:160px;
	width:210px;
}
.ride-quarter
{
	height:80px;
	width:210px;
	overflow: hidden;
}
.ride-info-image
{
	height:60px;
	width:40px;
	/*background:url('Resources/svg/location.svg');*/
	background-repeat: no-repeat;
	background-size: 32px;
	background-position:0px 0px;
	margin-bottom: 20px;
	float:left;
}
.ride-info-data
{
	height:80px;
	width:170px;
	float:left;
}
.ride-info-data-head
{
	width:170px;
	font-size:1.14em;
	line-height:1.14em;
	font-family: Roboto;
	color: rgba(0,0,0,0.87);
	max-height: 60px;
	overflow: hidden;
	-webkit-font-smoothing:antialiased;
}
.ride-info-data-caption
{
	margin-top:7px;
	font-size:0.85714285714286em;
	font-family: Roboto;
	color: rgba(0,0,0,0.54);
	text-transform: uppercase;
	width:170px;
}
.topWindow
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
.confirmationContainer
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
.confirmationButtons
{
	float:left;
	margin:9px;
	margin-top:0;
	margin-bottom:18px;
	width:115px;
}
.COCOne
{
	background:url('/Resources/images/path.png');
	width:100%;
	height:200px;
	position:relative;
}
.COCTwo
{
	border-bottom:solid 1px rgba(0,0,0,0.12);
	width:770px;
	height:32px;

	padding-top:34px;
	padding-bottom:33px;
	padding-left:15px;
	padding-right:15px;
}
.COCThree
{
	float:left;
	padding:15px;
	width:770px;
	height:70px;
	font-size:14px;
	line-height:20px;
	color:rgba(0,0,0,0.87);
	-webkit-font-smoothing: antialiased;
}
.confirmBox1
{
	float:left;
	height: 32px;
	display:inline-block;
	padding-right:7px;
	border-right:solid 1px rgba(0,0,0,0.12);
}
.confirmBox2
{
	float:left;
	height: 32px;
	display:inline-block;
	padding-left:8px;
	padding-right:7px;
	border-right:solid 1px rgba(0,0,0,0.12);
}
.confirmBox3
{
	height: 32px;
	display:inline-block;
	padding-left:8px;
}
.confirmationDetailsMark
{
	width:200px;
	margin-left:14px;
	float:left;
}
.COCFour
{
	width:100%;
	height:50px;
}
.Graphic-location-left
{	
	display: inline-block;
	position:absolute;
	top:27px;
	left:110px;
	text-align:left;
}
.Graphic-location-right
{
	display: inline-block;
	position:absolute;

	top:70px;
	right:119px;
	text-align:right;
}
.Head-graphic-location
{
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
#confirmationButtonsContainer
{
	width:262px;
	height:50px;
	margin-left:auto;
	margin-right:auto;
}
.no-ride-found-message
{
	margin-top:25px;
	font-size:14px;
	padding:16px;
	padding-left:108px;
	padding-top:30px;
	padding-bottom:30px;
	line-height:25px;
	color:rgba(0,0,0,0.54);
	-webkit-font-smoothing: antialiased;
	background-color:rgba(0,0,0,.025);
	-webkit-transition: all 250ms;
	moz-transition: all 250ms;
	width:556px;
	margin-left:auto;
	margin-right:auto;
	background-image:url('Resources/svg/wrong.svg');
	background-repeat:no-repeat;
	background-position:29px center;
	background-size:50px;
}
.center-filter-head
{
	margin-left:auto;
	margin-right:auto;
	height:60px;
	width:200px;
	opacity:0.87;
}
.filter-line0
{
	height:60px;
	width:100%;
	background-color:#2196F3;
	border-bottom: solid 1px rgba(0,0,0,0.12);
}
.filter-head-image
{
	float:left;
	height:60px;
	width:20px;
	background-image: url('Resources/images/Material_Search.png');
	background-size: 20px 20px;
	background-repeat: no-repeat;
	background-position: center center;
}
.filter-head-text
{
	margin-left:10px;
	float:left;
	display:inline-block;
	line-height: 60px;
	color:rgb(255,255,255);
	text-transform: uppercase;
	font-size:20px;
	-webkit-font-smoothing: antialiased;
	font-weight: 500;
}
.verticalFormSpecs
{
	width:275px;
}
.actionable-filters
{
	padding:10px;
	padding-left:37.5px;
	padding-right:37.5px;
}
</style>
</head>
<script>
	/* prefetch images */
	Image1= new Image(32,32);
	Image1.src = "Resources/svg/no_internet.svg";
</script>
<script> /* Fix boarding and destination */
	function fixDestination()
	{
		var boardingPoint = document.getElementById('boardingPoint').value;
		var op = document.getElementById("destination").getElementsByTagName("option");
		for (var i = 0; i < op.length; i++)
		{
	  		if(op[i].value == boardingPoint) 
	  		{
	  			if (boardingPoint != "")
	  			{
	  				op[i].disabled = true;
	  			}
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
	  			if (destination != "")
	  			{
		  			op[i].disabled = true;
		  		}
	  		}
	  		else
	  		{
	  			op[i].disabled = false;
	  		}
		}
	}
</script>
<script>
function viewRequest(requestID)
{
	document.getElementById("requestWindow").style.display = "block";
	document.getElementById("requestContainer").innerHTML = "<svg class='center spinner' style='margin-left:384px;margin-top:193px' width='32px' height='32px' viewBox='0 0 66 66' xmlns='http://www.w3.org/2000/svg'><circle class='path' fill='none' stroke-width='6' stroke-linecap='round' cx='33' cy='33' r='30'></circle></svg>";

	xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function () {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			var Reply = xmlhttp.responseText;
			document.getElementById("requestContainer").innerHTML = Reply;
		}
	}
	xmlhttp.open("POST","getRequestDetails.php",true);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("requestID=" + requestID);

	xmlhttp.onerror = function() {
		setTimeout(function(){ document.getElementById("requestContainer").innerHTML = "<div style='margin-left:auto;margin-right:auto;max-width:200px;padding-top:50px;'><img src='Resources/svg/no_internet.svg' style='min-width:200px;min-height:200px;margin-left:auto;margin-right:auto;'></div><div style='margin-left:100px;margin-right:100px;margin-top:50px;color:rgba(0,0,0,0.30);-webkit-font-smoothing: antialiased;line-height:18px;font-size:14px;'>Looks like your connection to the internet has been lost. There might be an issue with your Network settings. <span style='cursor:pointer' onclick='viewRequest(\"" + requestID +  "\")' class='accent'>Retry?</span></div><div onclick='cancelRequest()'' class='header-button' style='display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;''>close</div>"; }, 500);		
    };

	setTimeout(function(){ document.getElementById("requestWindow").style.opacity = "1"; }, 0);
	setTimeout(function(){ document.getElementById("requestContainer").style.boxShadow = "0 19px 38px rgba(0,0,0,0.30), 0 15px 12px rgba(0,0,0,0.22)"; }, 100);
}
function acceptRequest(requestID)
{
	document.getElementById("requestContainer").innerHTML = "<svg class='center spinner' style='margin-left:384px;margin-top:193px' width='32px' height='32px' viewBox='0 0 66 66' xmlns='http://www.w3.org/2000/svg'><circle class='path' fill='none' stroke-width='6' stroke-linecap='round' cx='33' cy='33' r='30'></circle></svg>";

	xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function () {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			var Reply = xmlhttp.responseText;
			document.getElementById("requestContainer").innerHTML = Reply;
		}
	}
	xmlhttp.open("POST","acceptRequest.php",true);
	xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
	xmlhttp.send("requestID=" + requestID);

	xmlhttp.onerror = function() {
		setTimeout(function(){ document.getElementById("requestContainer").innerHTML = "<div style='margin-left:auto;margin-right:auto;max-width:200px;padding-top:50px;'><img src='Resources/svg/no_internet.svg' style='min-width:200px;min-height:200px;margin-left:auto;margin-right:auto;'></div><div style='margin-left:100px;margin-right:100px;margin-top:50px;color:rgba(0,0,0,0.30);-webkit-font-smoothing: antialiased;line-height:18px;font-size:14px;'>Looks like your connection to the internet has been lost. There might be an issue with your Network settings. <span style='cursor:pointer' onclick='viewRequest(\"" + requestID +  "\")' class='accent'>Retry?</span></div><div onclick='cancelRequest()'' class='header-button' style='display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;''>close</div>"; }, 500);		
    };
}
function cancelRequest()
{
	document.getElementById("requestContainer").style.boxShadow = "0px 0px 0px rgb(0,0,0)";
	document.getElementById("requestWindow").style.opacity = "0";
	setTimeout(function(){ 	document.getElementById("requestContainer").innerHTML = ""; document.getElementById("requestWindow").style.display = "none"; }, 500);

}

/* Load date */
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
<body onload="loadDate();fixBoarding();fixDestination();">
	<?php include 'header.php'; ?>

	<div id="requestWindow" class="topWindow">
		<div id="requestContainer" class="confirmationContainer">
		</div>
	</div>

	<div class="everything-container clearfix">
		<div class="headline">Lift Requests</div>
		<div class="sub-line">Before you post an offer for your lift, please have a look at the lift requests below. You can directly offer a lift to someone instead of posting your offer.</div>

		<!--********** FILTERS **********-->

		<div class="filters-container">
			<div class="filter-line0">
				<div class="center-filter-head">
					<div class="filter-head-image"></div>
					<div class="filter-head-text">Search Options</div>
				</div>
			</div>
			<div class="filter-border"></div>
			<div class="actionable-filters">
				<select onblur="fixDestination();" id="boardingPoint" style="margin-top:12.5px;" class="select verticalFormSpecs object">
					<?php

						/* Populate Boarding Points*/

						$con = mysqli_connect("localhost","garvitgu_C", "Johny!@#PING", "garvitgu_Commute");
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
								echo '<option value="" selected="">All Places</option>';
							}
							else
							{
								echo '<option value="">All Places</option>';
							}
						}
					?>
				</select>
				<div id="boardingPoint-error" class="hint hint-fit">BOARDING POINT</div>
			
				<select onblur="fixBoarding();" id="destination" class="select verticalFormSpecs object">
					<?php
						/* Populate Destination Points*/

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
								echo '<option value="" selected="">All Places</option>';
							}
							else
							{
								echo '<option value="">All Places</option>';
							}
						}
					?>
				</select>
				<div id="destination-error" class="hint hint-fit">DESTINATION</div>

				<input class="object verticalFormSpecs" type="text" id="hbdsdf" placeholder="Date"></input>
				<div id="hbdsdf-error" class="hint hint-fit">DATE - ON OR AFTER</div>

				<select id="time" class="select verticalFormSpecs object">
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
				<div id="time-error" class="hint hint-fit">TIME - AT OR AFTER</div>
			</div>
		</div>

		<div class="right-container">
			<div class="notif-top">
				<span class="text-black">If you do not find these lift requests suitable, please </span>
				<?php

					$boardingPoint = htmlspecialchars(utf8_encode($_GET["From"]));
					$destination = htmlspecialchars(utf8_encode($_GET["To"]));
					$boardingDate = htmlspecialchars(utf8_encode($_GET["Date"]));
					$boardingTime = htmlspecialchars(utf8_encode($_GET["Time"]));
					echo '<a href="offerRide.php?From=' . $boardingPoint . '&To=' . $destination . '&Date=' . $boardingDate . '&Time=' . $boardingTime . '&Capacity=1">';
				?>

				<span class="bold accent">offer a new lift.</span></a>
			</div>
			<div id="availableRequests">
				<?php include 'showRequests.php'; ?>
			</div>
		</div>
	</div>

	<?php include 'footer.php'; ?>
	<script src="Resources/calender/ios-7-date-picker.js"></script>
	<script>
		$(document).on('change','#boardingPoint',function(){ refreshResult(); });
		$(document).on('change','#destination',function(){ refreshResult(); });
		$(document).on('change','#hbdsdf',function(){ refreshResult(); });
		$(document).on('change','#time',function(){ refreshResult(); });
		$(document).on('click','.mArea',function(){ refreshResult(); });

		function refreshResult()
		{	
			var boardingCode = $("#boardingPoint").val();
			var destination = $("#destination").val();
			var time = $("#time").val();
			var date = $("#hbdsdf").val();

			if ((date == "") || (date.localeCompare("Date") == 1) || (date == "Date"))
			{
				date = "1970 January 1";
			}

			document.getElementById("availableRequests").innerHTML =  "<svg class='center spinner' style='margin-left:384px;margin-top:193px' width='32px' height='32px' viewBox='0 0 66 66' xmlns='http://www.w3.org/2000/svg'><circle class='path' fill='none' stroke-width='6' stroke-linecap='round' cx='33' cy='33' r='30'></circle></svg>";
			
			xmlhttp = new XMLHttpRequest();
			xmlhttp.onreadystatechange = function () {
				if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
					document.getElementById("availableRequests").innerHTML = xmlhttp.responseText;
				}
			}
			xmlhttp.open("GET","showRequests.php?From=" + boardingCode + "&To=" + destination + "&Date=" + date + "&Time=" + time, true);
			xmlhttp.send();
		}
	</script>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
</body>
</html>