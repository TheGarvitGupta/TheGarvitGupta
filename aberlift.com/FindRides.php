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
	#parent
	{
		text-align:center;
		position: relative;
		margin-top:10%;
		top:0%;
		margin-bottom:10%;
	}
	#Message
	{
		background-color:rgb(230,230,230);
		font-family: 'Segoe UI Local', 'Segoe WP', 'Segoe UI Web', Tahoma, 'Helvetica Neue', Helvetica, 'Meiryo UI', Meiryo, 'Arial Unicode MS', sans-serif;
		font-weight:100;	
		margin-right: 10%;
		margin-left:10%;
		padding-top:10px;
		padding-bottom: 10px;
		padding-left:10px;
		padding-right:10px;
	}
	.CellParent
	{
		padding:10px;
		width:80%;
		background-color:rgb(240,240,240);
		margin-left:auto;
		margin-right:auto;		
		font-family: 'Segoe UI Local', 'Segoe WP', 'Segoe UI Web', Tahoma, 'Helvetica Neue', Helvetica, 'Meiryo UI', Meiryo, 'Arial Unicode MS', sans-serif;
				font-weight:100;	
		font-size:32px;
		-webkit-font-smoothing: antialiased;
		margin-bottom: 25px;
		box-shadow: 0px 0px 15px rgb(100,100,100);
	}
	.TableText
	{
		font-family: Helvetica, 'Meiryo UI', Meiryo, 'Arial Unicode MS', sans-serif;
				font-weight:100;	
		font-size:0.8em;
		-webkit-font-smoothing: antialiased;
		border:none;
		padding:0;
		margin:0;
	}
	.Pick
	{
		background-color:black;
		color:rgb(250,250,250);
				font-family: Helvetica, 'Meiryo UI', Meiryo, 'Arial Unicode MS', sans-serif;
				font-weight:100;	
		font-size:28px;
		-webkit-font-smoothing: antialiased;
		cursor:pointer;
		padding-top:5px;
		padding-bottom:5px;
		margin-top:10px;
		transition: 0.3s;
	}
	.Pick:hover
	{
		background-color: rgb(50,50,50);
	}
</style>
<script>
function SelectTrip(ID)
{
	
	xmlhttp = new XMLHttpRequest();
	xmlhttp.onreadystatechange = function () {
		if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
			var Reply = xmlhttp.responseText;
			if (1)
			{
				document.getElementById("parent").innerHTML="<div id='Message'>We have reserved your ride. You will shortly receive the details of your driver. Make sure you check your mail. Your trip ID is: <b>" + ID + "</b></div>";
				document.getElementById("parent").style.marginTop = "-10px";
				document.getElementById("parent").style.top = "50%";
				document.getElementById("parent").style.marginBottom = "auto";

			}
		}
	}
   xmlhttp.open("GET", "SelectTrip.php?TripID=" + ID, true);
   xmlhttp.send();
}
</script>
</head>
<body>
	<div id="parent">
		<?php
			include 'DB_Connect.php';

		  	$Query = "SELECT * FROM Commute_TRIPS";
			
			$Result = mysqli_query($con,$Query);

			$NumberOfRows = mysqli_num_rows($Result);

			for ($i=0; $i<$NumberOfRows; $i++)
			{
				$row = mysqli_fetch_array($Result);
				
				$From = $row['Trip_From'];
				$To = $row['Trip_To'];
				$Date = $row['Trip_Date'];
				$Time = $row['Trip_Time'];
				$Capacity = $row['Trip_Capacity'];
				$TripID = $row['Trip_ID'];

				//$FormattedDate = date("D, d F Y", strtotime($Date));
				$FormattedDate = gmdate("Y F j", $Date);
				$FormattedTime = date("h:i A", strtotime($Time));

				if ($From == 'AT')
				{
					$From = 'Aberystwyth Town (Train Station)';
				}
				else if ($From == 'GC')
				{
					$From = 'Gogerddan Campus';
				}
				else if ($From == 'LC')
				{
					$From = 'Llanbadarn Campus';
				}
				else if ($From == 'PC')
				{
					$From = 'Penglais Campus';
				}
				else if ($From == 'PV')
				{
					$From = 'Penparcau Village';
				}
				else
				{
					$From = 'Boarding Point';
				}

				if ($To == 'AT')
				{
					$To = 'Aberystwyth Town (Train Station)';
				}
				else if ($To == 'GC')
				{
					$To = 'Gogerddan Campus';
				}
				else if ($To == 'LC')
				{
					$To = 'Llanbadarn Campus';
				}
				else if ($To == 'PC')
				{
					$To = 'Penglais Campus';
				}
				else if ($To == 'PV')
				{
					$To = 'Penparcau Village';
				}
				else
				{
					$To = 'Boarding Point';
				}

				echo"<div class='CellParent'>";
				echo"<table class='TableText' style=\"background-color:rgb(220,220,220);padding:5px;text-align:center;width:100%;\">";
				echo"<tr style='width:100%;'>";
				echo"<td style='width:45%'>$From</td>";
				echo"<td style='width:10%'><img src='Resources/images/Material_Right.png'></td>";
				echo"<td style='width:45%'>$To</td>";
				echo"</tr>";
				echo"</table>";
				echo"<table class='TableText' style='margin-top:10px;text-align:center;width:100%;'>";
				echo"<tr style='width:100%;'>";
				echo"<td style='width:50%;color:rgb(80,80,80)'>$FormattedDate</td>";
				echo"<td style='width:50%;color:rgb(80,80,80)'>$FormattedTime</td>";
				echo"</tr>";
				echo"</table>";
				echo"<div onclick='SelectTrip(\"$TripID\")' class='Pick'>";
				echo"Select";
				echo"</div>";
				echo"</div>";
				/*
					Bogus text to increase size.
				*/
			}
		?>
	</div>
</body>
</html>