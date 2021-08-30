<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php

	$requestID = htmlspecialchars(utf8_encode($_POST["requestID"]));
	include 'DB_Connect.php';
	$Query = "SELECT * FROM AberLift_REQUESTS WHERE request_id='$requestID'";
	$result = mysqli_query($con,$Query);
	if (!empty($result))
	{
		$row = mysqli_fetch_array($result);

		echo '		<div class="COCOne">';
		echo '		<div class="Graphic-location-left">';
		echo '			<div class="Head-graphic-location">';
							$stationCodeQUERY = $row['from_code'];

							$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$stationCodeQUERY'";
							$resultint = mysqli_query($con, $Query);
							if (!empty($resultint))
							{
								$rowint = mysqli_fetch_array($resultint);
								echo $rowint['Station_Name'];
							}
		echo '			</div>';
		echo '			<div class="Sub-graphic-location">Boarding Point</div>';
		echo '		</div>';
		echo '		<div class="Graphic-location-right">';
		echo '			<div class="Head-graphic-location">';
							$stationCodeQUERY = $row['to_code'];

							$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$stationCodeQUERY'";
							$resultint = mysqli_query($con, $Query);
							if (!empty($resultint))
							{
								$rowint = mysqli_fetch_array($resultint);
								echo $rowint['Station_Name'];
							}
		echo '			</div>';
		echo '			<div class="Sub-graphic-location">Destination</div>';
		echo '		</div>';
		echo '		</div>';
		echo '		<div class="COCTwo">';
		echo '			<div class="confirmBox1">';

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

		echo "				<div style=\"border-radius:100%;float:left;height:32px;width:32px;background:url('" . $picture . "');background-repeat:no-repeat;background-size:cover;background-position:center;\"></div>";
		echo '				<div class="confirmationDetailsMark">';
		echo '					<div class="Head-graphic-location">';
		echo 						$rowint['firstName'] . " " . $rowint['lastName'];
		echo '					</div>';
		echo '					<div class="Sub-graphic-location">REQUESTED BY</div>';
		echo '				</div>';
		echo '			</div>';
		echo '			<div class="confirmBox2">';
		echo '				<div style="float:left;height:32px;width:32px;background:url(\'Resources/svg/time.svg\');background-repeat:no-repeat;background-size:32px;background-position:0px 0px;"></div>';
		echo '				<div class="confirmationDetailsMark">';
		echo '					<div class="Head-graphic-location">';

								    $x = date_parse_from_format('Hi', $row['ride_time']);
									$timeInEpoch = mktime($x['hour'], $x['minute'], $x['second'], $x['month'], $x['day'], $x['year'], $x['is_dst']);
									echo date('H:i', $timeInEpoch);

		echo '					</div>';
		echo '					<div class="Sub-graphic-location">Time</div>';
		echo '				</div>';
		echo '			</div>';
		echo '				<div class="confirmBox3">';
		echo '					<div style="float:left;height:32px;width:32px;background:url(\'Resources/svg/calender.svg\');background-repeat:no-repeat;background-size:32px;background-position:0px 0px;"></div>';
		echo '					<div class="confirmationDetailsMark">';
		echo '						<div class="Head-graphic-location">';

										echo date('j F Y', $row['ride_date']);

		echo '						</div>';
		echo '						<div class="Sub-graphic-location">Date</div>';
		echo '					</div>';
		echo '				</div>';
		echo '			</div>';
		echo '			<div class="COCThree">';
		echo '				Please have a look at the request carefully before accepting it. Once you accept the request, we\'ll send your details to ';

							$requester_id = $row['requester_id'];
							$Query = "SELECT * FROM AberLift_USERS WHERE userID = '$requester_id'";
							$resultint = mysqli_query($con, $Query);
							if (!empty($resultint))
							{
								$rowint = mysqli_fetch_array($resultint);
								echo $rowint['firstName'] . " " . $rowint['lastName'];
							}

		echo '				and send their details to you. By clicking the accept button below, you agree to abide by AberLift\'s <span class="accent">code of conduct </span>and acknowledge that AberLift would not be responsible if something goes wrong. ';
		echo '			</div>';
		echo '			<div class="COCFour">';
		echo '				<div style="width:297px" id="confirmationButtonsContainer">';
		echo '					<div style="width:150px" onclick="acceptRequest(\'' . $requestID . '\')" class="header-button confirmationButtons">Accept Request</div>';
		echo '					<div onclick="cancelRequest()" class="center header-button confirmationButtons" style="width:65px">Cancel</div>';
		echo '				</div>';
		echo '			</div>';
	}
?>