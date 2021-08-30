<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php

	$requesterID = $_SESSION["UserID"];

	include 'DB_Connect.php';
	$Query = "SELECT * FROM AberLift_REQUESTS WHERE requester_id = '$requesterID' ORDER BY ride_date DESC, ride_time DESC";
	$result = mysqli_query($con,$Query);
	if (mysqli_num_rows($result) > 0)
	{
		while($row = mysqli_fetch_array($result))
		{	
			$stationCodeQUERY = $row['from_code'];

			$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$stationCodeQUERY'";
			$resultint = mysqli_query($con, $Query);
			if (!empty($resultint))
			{
				$boardingStationINT = mysqli_fetch_array($resultint);
			}

			$stationCodeQUERY = $row['to_code'];

			$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$stationCodeQUERY'";
			$resultint = mysqli_query($con, $Query);
			if (!empty($resultint))
			{
				$destinationINT = mysqli_fetch_array($resultint);
			}

			$x = date_parse_from_format('Hi', $row['ride_time']);
			$timeInEpoch = mktime($x['hour'], $x['minute'], $x['second'], $x['month'], $x['day'], $x['year'], $x['is_dst']);

			// Set Status
			if ($row['status'] == "active")
			{
				$status = "OPEN";
			}
			else
			{
				$status = "CONFIRMED";
			}

			echo '<div class="individual">';
			echo '<div class="individual-item">';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:150px">' . date('D, d M Y', $row['request_timestamp']) . '</div><div class="bottom-head-weak">REQUEST DATE</div></div>';
			echo '</div>';
			echo '<div class="individual-item">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/location.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:170px">' . $boardingStationINT['Station_Name'] . '</div><div class="bottom-head-weak">BOARD FROM</div></div>';
			echo '</div>';
			echo '<div class="individual-item">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/location.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:170px">' . $destinationINT['Station_Name'] . '</div><div class="bottom-head-weak">LIFT TILL</div></div>';
			echo '</div>';
			echo '<div class="individual-item">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/calender.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:115px">' . date('j F Y', $row['ride_date']) . '</div><div class="bottom-head-weak">LIFT Date</div></div>';
			echo '</div>';
			echo '<div class="individual-item">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/time.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:50px">' . date('H:i', $timeInEpoch) . '</div><div class="bottom-head-weak">LIFT TIME</div></div>';
			echo '</div>';
			echo '<div class="individual-item-last">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/calender.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:90px">' . $status . '</div><div class="bottom-head-weak">STATUS</div></div>';
			echo '</div>';
			echo '</div>';
		}
	}
	else
	{
		echo "Nothing to show here. You have not requested any lifts yet.";
	}
?>