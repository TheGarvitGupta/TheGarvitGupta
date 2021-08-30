<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php
	$From = htmlspecialchars(utf8_encode($_GET["From"])) . '%';
	$To = htmlspecialchars(utf8_encode($_GET["To"])) . '%';
	$Date = htmlspecialchars(utf8_encode($_GET["Date"]));
	$Time = htmlspecialchars(utf8_encode($_GET["Time"]));

	/* Check if date is valid */
    $x = date_parse_from_format('Y F j', $Date);

	if (!($x["error_count"] == 0) || (($From == $To) && ($To != "%")))
	{
		echo "<div class='no-ride-found-message'>Ah! There was an error in the input we received. Please try again.<br>Error: Invalid date or the selected Aber points are same.</div>";	
		exit();
	}
	else /* Details are correct */ /* Echo SUCCESS */
	{
		/* Build timestamp from date */
		$Date = mktime($x['hour'], $x['minute'], $x['second'], $x['month'], $x['day'], $x['year'], $x['is_dst']);
	}

	include 'DB_Connect.php';
	$Query = "SELECT * FROM AberLift_REQUESTS WHERE status='active' AND from_code LIKE '$From' AND to_code LIKE '$To' AND ride_date >=$Date AND ride_time >=$Time ORDER BY ride_date, ride_time, request_timestamp ";
	$result = mysqli_query($con,$Query);
	if (mysqli_num_rows($result) > 0)
	{
		while($row = mysqli_fetch_array($result))
		{	
			echo'<div onclick="viewRequest(\'' . $row['request_id'] . '\')" class="ride-container">';
			echo'<div class="ride-person-container">';

			/* Get person's picture. If none exists, show a generic picture*/

			$requester_id = $row['requester_id'];
			$Query = "SELECT * FROM AberLift_USERS WHERE userID = '$requester_id'";
			$resultint = mysqli_query($con, $Query);
			if (!empty($resultint))
			{
				$rowint = mysqli_fetch_array($resultint);
				$picture = 'uploads/' . $rowint['profilePicture'];
			}

			if ((!(file_exists($picture))) || $rowint['profilePicture'] == "" || $rowint['profilePicture'] == "null" || $rowint['profilePicture'] == null)
			{
				$picture = 'Resources/images/person_generic.png';
			}

			/************************************************/

			echo"<div class='ride-picture-container' style=\"background-image:url('" .  $picture .  "')\"></div>";
			echo'<div class="ride-text-container">';
			
			/* echo requester name using requester_id */

			echo $rowint['firstName'] . " " . $rowint['lastName'];

			echo '</div>';
			echo'</div>';
			echo'<div class="ride-info-container">';
			echo'<div class="ride-info-section mright">';
			echo'<div class="ride-quarter">';
			echo'<div class="ride-info-image" style="background:url(\'Resources/svg/location.svg\');background-repeat: no-repeat;background-size: 32px;background-position:0px 0px;"></div>';
			echo'<div class="ride-info-data">';
			echo'<div class="ride-info-data-head">';
			
			/* echo station name using station code */
			/* variable appended with int(ermediary) to prevent global ones from being used */
			$stationCodeQUERY = $row['from_code'];

			$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$stationCodeQUERY'";
			$resultint = mysqli_query($con, $Query);
			if (!empty($resultint))
			{
				$rowint = mysqli_fetch_array($resultint);
				echo $rowint['Station_Name'];
			}

			echo'</div>';
			echo'<div class="ride-info-data-caption">Boarding Point</div>';
			echo'</div>';
			echo'</div>';
			echo'<div class="ride-quarter">';
			echo'<div class="ride-info-image" style="background:url(\'Resources/svg/location.svg\');background-repeat: no-repeat;background-size: 32px;background-position:0px 0px;"></div>';
			echo'<div class="ride-info-data">';
			echo'<div class="ride-info-data-head">';
			
			/* echo station name using station code */
			/* variable appended with int(ermediary) to prevent global ones from being used */
			$stationCodeQUERY = $row['to_code'];

			$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$stationCodeQUERY'";
			$resultint = mysqli_query($con, $Query);
			if (!empty($resultint))
			{
				$rowint = mysqli_fetch_array($resultint);
				echo $rowint['Station_Name'];
			}

			echo'</div>';
			echo'<div class="ride-info-data-caption">Destination</div>';
			echo'</div>';
			echo'</div>';
			echo'</div>';
			echo'<div class="ride-info-section">';
			echo'<div class="ride-quarter">';
			echo'<div class="ride-info-image" style="background:url(\'Resources/svg/calender.svg\');background-repeat: no-repeat;background-size: 32px;background-position:0px 0px;"></div>';
			echo'<div class="ride-info-data">';
			echo'<div class="ride-info-data-head">';

			/* parse date to correct format */
			echo date('j F Y', $row['ride_date']);

			echo'</div>';
			echo'<div class="ride-info-data-caption">Lift Date</div>';
			echo'</div>';
			echo'</div>';
			echo'<div class="ride-quarter">';
			echo'<div class="ride-info-image" style="background:url(\'Resources/svg/time.svg\');background-repeat: no-repeat;background-size: 32px;background-position:0px 0px;"></div>';
			echo'<div class="ride-info-data">';
			echo'<div class="ride-info-data-head">';

			/* parse time to correct format */
		    $x = date_parse_from_format('Hi', $row['ride_time']);
			$timeInEpoch = mktime($x['hour'], $x['minute'], $x['second'], $x['month'], $x['day'], $x['year'], $x['is_dst']);
			echo date('H:i', $timeInEpoch);

			echo'</div>';
			echo'<div class="ride-info-data-caption">Lift Time</div>';
			echo'</div>';
			echo'</div>';
			echo'</div>';
			echo'</div>';
			echo'</div>';
		}
	}
	else
	{
		$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code LIKE '$From'";
		$result = mysqli_query($con, $Query);
		$row = mysqli_fetch_array($result);
		$ST1Name = $row['Station_Name'];

		$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code LIKE '$To'";
		$result = mysqli_query($con, $Query);
		$row = mysqli_fetch_array($result);
		$ST2Name = $row['Station_Name'];

		if ($ST1Name == "" || $ST1Name == null || $ST2Name == "" || $ST2Name == null)
		{
			echo "<div class='no-ride-found-message'>Ah! There was an error in the input we received. Please try again.<br>Error: Station codes were not recognized.</div>";
		}

		if (($From == "%") && ($To == "%"))
		{
			echo "<div class='no-ride-found-message'>Looks like nobody has posted a request for a lift for the selected date and time. You can try searching for a different date and time or publish your lift offer. We'll let you know as soon as someone requests to share the lift with you.</div>";
		}
		else if ($From == "%")
		{
			echo "<div class='no-ride-found-message'>Looks like nobody has posted a request for a lift to " . $ST2Name . " for the selected date and time. You can try searching for a different date and time or publish your lift offer. We'll let you know as soon as someone requests to share the lift with you.</div>";
		}
		else if ($To == "%")
		{
			echo "<div class='no-ride-found-message'>Looks like nobody has posted a request for a lift from " . $ST1Name . " for the selected date and time. You can try searching for a different date and time or publish your lift offer. We'll let you know as soon as someone requests to share the lift with you.</div>";
		}
		else
		{
			echo "<div class='no-ride-found-message'>Looks like nobody has posted a request for a lift from " . $ST1Name . " to " . $ST2Name . " for the selected date and time. You can try searching for a different date and time or publish your lift offer. We'll let you know as soon as someone requests to share the lift with you.</div>";
		}
	}
?>