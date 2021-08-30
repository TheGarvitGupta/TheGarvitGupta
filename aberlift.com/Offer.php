<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<meta name="viewport" content="width=device-width, initial-scale=1">
<html>
<head>
<link rel="stylesheet" type="text/css" href="Style.css">
<style>
	#Message
	{
		background-color:rgb(230,230,230);
		font-family: 'Segoe UI Local', 'Segoe WP', 'Segoe UI Web', Tahoma, 'Helvetica Neue', Helvetica, 'Meiryo UI', Meiryo, 'Arial Unicode MS', sans-serif;
		margin-right: 10%;
		margin-left:10%;
		padding-top:10px;
		padding-bottom: 10px;
		padding-left:10px;
		padding-right:10px;
	}
</style>
<script>
function OfferRide()
{
	var e = document.getElementById("From");
	var From = e.options[e.selectedIndex].value;
	e = document.getElementById("To");
	var To = e.options[e.selectedIndex].value;
	var date = document.getElementById("date").value;
	var Time = document.getElementById("time").value;
	e = document.getElementById("Capacity");
	var Capacity = e.options[e.selectedIndex].value;

	xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function () {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			var Reply = xmlhttp.responseText;
			if (Reply == '1')
			{
				document.getElementById("parent").innerHTML="<div id='Message'>We have received your ride information. We'll Email you once we find a rider matching the details of your travel.</div>";
				document.getElementById("parent").style.marginTop = "250px";
			}
		}
	}
   xmlhttp.open("GET", "OfferRide.php?From=" + From + "&To=" + To + "&Date=" + date + "&Time=" + Time+ "&Capacity=" + Capacity, true);
   xmlhttp.send();
}
</script>
</head>
<body class="Back">
	<div id="Top">
		<div id="AppName">LifterApp<sup style="font-size:15px">PREVIEW</sup></div>
	</div>
	<div id="parent">
		<select id="From" class="object textbox select">
			<option value="" disabled selected>From:</option>
			<option value="AT">Aberystwyth Town (Train Station)</option>
			<option value="GC">Gogerddan Campus</option>
			<option value="LC">Llanbadarn Campus</option>
			<option value="PC">Penglais Campus</option>
			<option value="PV">Penparcau Village</option>
		</select>
		<select id="To" class="object textbox select">
			<option value="" disabled selected>To:</option>
			<option value="AT">Aberystwyth Town (Train Station)</option>
			<option value="GC">Gogerddan Campus</option>
			<option value="LC">Llanbadarn Campus</option>
			<option value="PC">Penglais Campus</option>
			<option value="PV">Penparcau Village</option>
		</select>
		<input type="date" id="date" placeholder="Date of Departure" class="textbox object"/><br>	
		<input type="time" id="time" placeholder="Time of Departure" class="textbox object"/><br>
		<select id="Capacity" class="object textbox select">
			<option value="" disabled selected>Number of available Seats</option>
			<option value="1">1</option>
			<option value="2">2</option>
			<option value="3">3</option>
		</select>
		<input type="button" onclick="OfferRide()" class="button object" value="Offer this ride"></input>
	</div>
	<div id="Bottom">
		<div id="FooterText">Contact Us - About Us - How to use - Privacy Policy - Help</div>
	</div>
</body>
</html>