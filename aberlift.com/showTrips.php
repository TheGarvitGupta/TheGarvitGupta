<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php

	$UserID = $_SESSION["UserID"];

	include 'DB_Connect.php';
	// Without Sorting on Date ad Time
	// $Query = "SELECT DISTINCT offer_id FROM AberLift_TRIPS WHERE trip_id IN (SELECT trip_id FROM AberLift_TRIPS WHERE request_id IN (SELECT request_id FROM AberLift_REQUESTS WHERE requester_id = '$UserID') OR offer_id IN (SELECT offer_id FROM AberLift_OFFERS WHERE offerer_id = '$UserID'))";
	$Query = "SELECT offer_id FROM AberLift_OFFERS WHERE offer_id IN(SELECT DISTINCT offer_id FROM AberLift_TRIPS WHERE trip_id IN (SELECT trip_id FROM AberLift_TRIPS WHERE request_id IN (SELECT request_id FROM AberLift_REQUESTS WHERE requester_id = '$UserID') OR offer_id IN (SELECT offer_id FROM AberLift_OFFERS WHERE offerer_id = '$UserID'))) ORDER BY ride_date DESC, ride_time DESC";
	$result = mysqli_query($con,$Query);
	if (mysqli_num_rows($result) > 0)
	{
		while($row = mysqli_fetch_array($result))
		{	
			/* For each offer id */
			$offer_id = $row['offer_id'];
			$Query2 = "SELECT * FROM AberLift_OFFERS WHERE offer_id = '$offer_id'";
			$result2 = mysqli_query($con,$Query2);
			$rowOfOfferDetails = mysqli_fetch_array($result2);

			$stationCodeQUERY = $rowOfOfferDetails['from_code'];

			$Query3 = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$stationCodeQUERY'";
			$resultint = mysqli_query($con, $Query3);
			if (!empty($resultint))
			{
				$boardingStationINT = mysqli_fetch_array($resultint);
			}

			$stationCodeQUERY = $rowOfOfferDetails['to_code'];

			$Query3 = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$stationCodeQUERY'";
			$resultint = mysqli_query($con, $Query3);
			if (!empty($resultint))
			{
				$destinationINT = mysqli_fetch_array($resultint);
			}

			$x = date_parse_from_format('Hi', $rowOfOfferDetails['ride_time']);
			$timeInEpoch = mktime($x['hour'], $x['minute'], $x['second'], $x['month'], $x['day'], $x['year'], $x['is_dst']);

			/* name and picture */

			$offerer_id = $rowOfOfferDetails['offerer_id'];
			$Query = "SELECT * FROM AberLift_USERS WHERE userID = '$offerer_id'";
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

			/*echo $rowint['firstName'] . " " . $rowint['lastName'];*/

			/*till here */

			echo '<div class="individual" style="height:74px;">';
			echo '<div class="individual-item">';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:110px">' . $rowOfOfferDetails['offer_id'] . '</div><div class="bottom-head-weak">OFFER ID</div></div>';
			echo '</div>';
			echo '<div class="individual-item">';
			echo '<div class="push-left-1" style="border-radius:50%;background-size:cover;background-position:center;background-image:url(\'' . $picture . '\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:125px">' . $rowint['firstName'] . " " . $rowint['lastName'] . '</div><div class="bottom-head-weak">DRIVER</div></div>';
			echo '</div>';
			echo '<div class="individual-item">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/location.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:170px">' . $boardingStationINT['Station_Name'] . '</div><div class="bottom-head-weak">Boarding Point</div></div>';
			echo '</div>';
			echo '<div class="individual-item">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/location.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:170px">' . $destinationINT['Station_Name'] . '</div><div class="bottom-head-weak">Destination</div></div>';
			echo '</div>';
			echo '<div class="individual-item">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/calender.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:115px">' . date('j F Y', $rowOfOfferDetails['ride_date']) . '</div><div class="bottom-head-weak">LIFT DATE</div></div>';
			echo '</div>';
			echo '<div class="individual-item-last">';
			echo '<div class="push-left-1" style="background-image:url(\'Resources/svg/time.svg\');"></div>';
			echo '<div class="push-left-2"><div class="top-head-strong" style="width:50px">' . date('H:i', $timeInEpoch) . '</div><div class="bottom-head-weak">LIFT TIME</div></div>';
			echo '</div>';
			echo' <div style="margin-left:10px;margin-top:10px;border:none;line-height:32px;" class="individual-item">';
			echo' <div class="float:left;line-height:32px;">PASSENGERS:</div>';
			echo' </div>';


			/* echo passengers */
			$ZQuery = "SELECT * FROM AberLift_REQUESTS WHERE request_id IN (SELECT request_id FROM AberLift_TRIPS WHERE offer_id = '$offer_id')";
			$Zresult = mysqli_query($con,$ZQuery);
			if (mysqli_num_rows($Zresult) > 0)
			{
				while($Zrow = mysqli_fetch_array($Zresult))
				{	
					/* name and picture */

					$offerer_idZ = $Zrow['requester_id'];
					$MQuery = "SELECT * FROM AberLift_USERS WHERE userID = '$offerer_idZ'";
					$Tresultint = mysqli_query($con, $MQuery);
					if (!empty($Tresultint))
					{
						$rowintA = mysqli_fetch_array($Tresultint);
						$pictureE = 'uploads/' . $rowintA['profilePicture'];
					}

					if ((!(file_exists($pictureE))) || $rowintA['profilePicture'] == "" || $rowintA['profilePicture'] == "null" || $rowintA['profilePicture'] == null)
					{
						$pictureE = 'Resources/images/person_generic.png';
					}

					echo '<div style="margin-left:10px;margin-right:0px;margin-top:10px;border:none" class="individual-item">';
					echo '<div class="push-left-1" style="border-radius:50%;background-size:cover;background-position:center;background-image:url(\'' . $pictureE . '\');"></div>';
					echo '<div class="push-left-2"><div class="top-head-strong" style="width:125px;overflow:none">' . $rowintA['firstName'] . " " . $rowintA['lastName'] . '</div><div class="bottom-head-weak">' . substr($rowintA['phoneNumber'], 2, 3)." ".substr($rowintA['phoneNumber'], 5, 3)." ".substr($rowintA['phoneNumber'],8) . '</div></div>';
					echo '</div>';
				}
			}

			/* end div */
			echo '</div>';
		}
	}
	else
	{
		echo "Nothing to show here. You have not been a part of any trips  yet.";
	}
?>