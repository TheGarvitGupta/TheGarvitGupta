<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<meta charset="UTF-8">
<html>
<head>
	<link href="Resources/css/home.css?s" rel="stylesheet" type="text/css">
	<script src='Resources/JS/jquery.min.js'></script> 
	<script src="Resources/JS/home.js?s"></script>

	<style type="text/css">

		/* Override Header Shadow */
		header
		{
			box-shadow: 0px 0px 0px;
		}
		.main-frame
		{
			min-height:285px;
		}
		.box-center
		{
			margin-left:auto;
			margin-right:auto;
		}
		.trips-page-container
		{
			width:1120px;
			padding:25px;
			margin-top:50px;
			margin-bottom:30px;
			box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);

			transition:all 250ms ease-out;
			-webkit-transition:all 250ms ease-out;
		}

		.tabs-bar
		{
			background-color:#2196F3;
			width:100%;
			margin-top:65px;
			height:45px;
			padding-top:10px;

			box-shadow: 0px 2px 5px rgba(0,0,0,0.258824);

			-webkit-touch-callout: none;
			-webkit-user-select: none;
			-khtml-user-select: none;
			-moz-user-select: none;
			-ms-user-select: none;
			user-select: none;
		}
		.tabs-container
		{
			width:1170px;
			margin-left:auto;
			margin-right:auto;
			height:45px;
		}

		.center-div
		{
			margin-left:auto;
			margin-right:auto;
			width:660px;
			height:45px;
		}

		.tab
		{
			height:45px;
			float:left;
			width:200px;
			margin-left:10px;
			margin-right:10px;
			text-align:center;
		}
		.tab-inner
		{
			height:42px;
			display:inline-block;
			padding-left:10px;
			padding-right:10px;
			margin-left:auto;
			margin-right:auto;

			/*border-bottom:solid 3px #F4FF81;*/
			/*color:rgb(255,255,255);*/

			color:rgba(255,255,255,0.70);
			cursor: pointer;

			font-family:Roboto;
			font-size: 16px;
			font-weight:500;
			text-align: center;
			-webkit-font-smoothing:antialiased;
			text-transform: uppercase;
			line-height:43px;

			transition:color 250ms ease-out;
			-webkit-transition:color 250ms ease-out;
		}
		.individual
		{
			height:32px;
			width:100%;

			padding-bottom:25px;
			margin-top:25px;
			border-bottom:solid 1px rgba(0,0,0,0.12);
		}
		.individual-item
		{
			height:32px;
			/*width:200px;*/
			padding-right:15px;
			border-right:solid 1px rgba(0,0,0,0.12);
			margin-right:14px;
			float:left;
		}
		.individual-item-last
		{
			height:32px;
			float:left;			
		}
		.push-left-1
		{
			float:left;
			height:32px;
			width:32px;
			background-repeat:no-repeat;
			background-size:32px;
			background-position:0px 0px;
		}
		.push-left-2
		{
			margin-left:10px;
			float:left;
		}
		.top-head-strong
		{
			height:20px;
			line-height:20px;
			font-size:16px;
			color:rgba(0,0,0,0.87);
			-webkit-font-smoothing: antialiased;

			overflow:hidden;
		}
		.bottom-head-weak
		{
			height:16px;
			line-height:16px;
			font-size: 11px;
			text-transform: uppercase;
			color:rgba(0,0,0,0.54);
			-webkit-font-smoothing: antialiased;
		}
	</style>
	<script>
	function offers()
	{
		document.getElementById("offers").style.borderBottom = "solid 3px #F4FF81";
		document.getElementById("offers").style.color = "rgb(255,255,255)";

		document.getElementById("requests").style.borderBottomWidth = "0px";
		document.getElementById("requests").style.color = "rgba(255,255,255,0.7)";

		document.getElementById("trips").style.borderBottomWidth = "0px";
		document.getElementById("trips").style.color = "rgba(255,255,255,0.7)";

		xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function () {
			if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
				document.getElementById("dynamic-container").innerHTML = xmlhttp.responseText;
			}
		}
		xmlhttp.open("GET","showTripOffers.php",true);
		xmlhttp.send();
	}

	function requests()
	{
		document.getElementById("requests").style.borderBottom = "solid 3px #F4FF81";
		document.getElementById("requests").style.color = "rgb(255,255,255)";

		document.getElementById("offers").style.borderBottomWidth = "0px";
		document.getElementById("offers").style.color = "rgba(255,255,255,0.7)";

		document.getElementById("trips").style.borderBottomWidth = "0px";
		document.getElementById("trips").style.color = "rgba(255,255,255,0.7)";

		xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function () {
			if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
				document.getElementById("dynamic-container").innerHTML = xmlhttp.responseText;
			}
		}
		xmlhttp.open("GET","showTripRequests.php",true);
		xmlhttp.send();
	}

	function trips()
	{
		document.getElementById("trips").style.borderBottom = "solid 3px #F4FF81";
		document.getElementById("trips").style.color = "rgb(255,255,255)";

		document.getElementById("offers").style.borderBottomWidth = "0px";
		document.getElementById("offers").style.color = "rgba(255,255,255,0.7)";

		document.getElementById("requests").style.borderBottomWidth = "0px";
		document.getElementById("requests").style.color = "rgba(255,255,255,0.7)";

		xmlhttp = new XMLHttpRequest();
		xmlhttp.onreadystatechange = function () {
			if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
				document.getElementById("dynamic-container").innerHTML = xmlhttp.responseText;
			}
		}
		xmlhttp.open("GET","showTrips.php",true);
		xmlhttp.send();
	}


	/* Top shadow maintain*/

	$(window).scroll(function (event) {
		var scroll = $(window).scrollTop();
		if (scroll >=55)
		{
			// Show Shadow
			$( "header" ).css( "boxShadow", "0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24)");
		}
		else
		{
			$( "header" ).css( "boxShadow", "0px 0px 0px");
		}
	});

	</script>
</head>
<body>
	<?php include 'header.php' ?>

	<div class="tabs-bar">
		<div class="tabs-container">
			<div class="center-div">
				<div class="tab">
					<div id="offers" onclick="offers()" class="tab-inner" style="border-bottom:solid 3px #F4FF81;color:rgb(255,255,255);">Offers</div>
				</div>
				<div class="tab">
					<div id="requests" onclick="requests()" class="tab-inner">Requests</div>
				</div>
				<div class="tab">
					<div id="trips" onclick="trips()" class="tab-inner">Confirmed Trips</div>
				</div>
			</div>
		</div>
	</div>

	<div class="main-frame">
		<div id="dynamic-container" class="box-center trips-page-container">

			<?php include 'showTripOffers.php' ?>

		</div>
	</div>

	<?php include 'footer.php' ?>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
</body>
</html>