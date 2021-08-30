<?php
	session_start();
	if (!(isset($_SESSION["LoggedIn"])))
	{
		header('Location: signin.php?Error=UserNotLoggedIn');
		exit();
	}
?>
<?php

	/* This script will be executed by the requester who accepts an already available offer from the list. He will send the offer ID of the offer and his own ID */

	include 'DB_Connect.php';

	$offerID = htmlspecialchars(utf8_encode($_POST["offerID"]));
	$requesterID = $_SESSION["UserID"];

	// get seatcapacity from offer

	$SQL = "SELECT * FROM AberLift_OFFERS WHERE offer_id = '$offerID'";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	$Result = mysqli_query($con,$SQL);
	$row = mysqli_fetch_array($Result);
	$seats = $row['seats'];

	$fromLocation = $row['from_code'];
	$toLocation = $row['to_code'];
	$rideDate = $row['ride_date'];
	$rideTime = $row['ride_time'];
	$timestamp = time();

	$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$fromLocation'";
	$resultint = mysqli_query($con, $Query);
	if (!empty($resultint))
	{
		$rowint = mysqli_fetch_array($resultint);
		$fromName = $rowint['Station_Name'];
	}

	$Query = "SELECT Station_Name FROM AberLift_STATIONS WHERE Station_Code='$toLocation'";
	$resultint = mysqli_query($con, $Query);
	if (!empty($resultint))
	{
		$rowint = mysqli_fetch_array($resultint);
		$toName = $rowint['Station_Name'];
	}

    $x = date_parse_from_format('Hi', $rideTime);
	$timeInEpoch = mktime($x['hour'], $x['minute'], $x['second'], $x['month'], $x['day'], $x['year'], $x['is_dst']);
	$formattedTime = date('H:i', $timeInEpoch);

	$formattedDate = date('j F Y', $rideDate);


	// Check that the requester has not already selected this offer in past

	$QUERY = "SELECT IF(COUNT(*) > 0, 'true', 'false') AS 'Answer' FROM AberLift_REQUESTS WHERE '$requesterID' IN (
				SELECT DISTINCT AberLift_REQUESTS.requester_id
				FROM AberLift_REQUESTS
				INNER JOIN AberLift_TRIPS
				ON AberLift_TRIPS.request_id = AberLift_REQUESTS.request_id
				WHERE AberLift_TRIPS.offer_id = '$offerID')";
	$resultint = mysqli_query($con, $QUERY);
	if (!empty($resultint))
	{
		$rowint = mysqli_fetch_array($resultint);
		$answer = $rowint['Answer'];
	}

	if ($answer == "true")
	{
		// This requester (id) has selected this offer (id) in the past
		echo "<img src='Resources/svg/warning.svg' style='min-width:200px;min-height:200px;margin-top:50px;margin-left:300px;margin-right:300px;'></div><div style='margin-left: 100px;margin-right: 100px;margin-top: 10px;color: rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;line-height: 18px;font-size: 14px;text-align:center;'><div style='text-align:center;font-size:18px;line-height:32px' class='accent'>Hey!</div><br>You have already accepted this offer. You can't select an offer twice!</div><div onclick='cancelOffer()' class='header-button' style='display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;'>close</div>";
		exit();
	}

	if ($seats <= 0)
	{
		echo "<img src='Resources/svg/error.svg' style='min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;'></div><div style='margin-left: 100px;margin-right: 100px;margin-top: 10px;color: rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;line-height: 18px;font-size: 14px;'><div style='text-align:center;font-size:18px;line-height:32px' class='accent'>Oops! It's gone.</div><br>Looks like somone grabbed this offer before you did. No worries, you can select from the available offers or post a new request for the lift.</div><div onclick='cancelOffer()' class='header-button' style='display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;'>close</div>";
		exit();
	}

	if ($seats == 1)
	{
		$status = "taken";
	}
	else
	{
		$status = "active";
	}

	$seats -= 1;

	// update offers table with seat capacity and status

	$SQL = "UPDATE AberLift_OFFERS SET seats='$seats', status='$status' WHERE offer_id='$offerID'";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}

	// generate request id
	function generateRandomString($length = 10) 
	{
		$characters = '0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ';
	    $charactersLength = strlen($characters);
	    $randomString = '';
	    for ($i = 0; $i < $length; $i++)
	    {
	        $randomString .= $characters[rand(0, $charactersLength - 1)];
	    }
	    return $randomString;
	}

	$newRequestID = generateRandomString();

	// insert into request table against requesterid

	$SQL = "INSERT INTO AberLift_REQUESTS VALUES('$timestamp', '$newRequestID', '$requesterID', '$fromLocation', '$toLocation', '$rideDate', '$rideTime', 'taken')";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	
	// generate trip id

	$tripID = generateRandomString();

	// insert trip id, request id and offer id
	$SQL = "INSERT INTO AberLift_TRIPS VALUES('$timestamp', '$tripID', '$newRequestID', '$offerID', 'positive')";
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}


	// communication data
	// requester
	$SQL = "SELECT * FROM AberLift_USERS WHERE userID = '$requesterID'";	
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	$Result = mysqli_query($con,$SQL);
	$row = mysqli_fetch_array($Result);

	$requesterName = $row['firstName'] . ' ' .  $row['lastName'];
	$requesterPhone = $row['phoneNumber'];
	$requesterEmail = $row['emailAddress'];

	// offerer
	// we have the offerid
	$SQL = "SELECT * FROM AberLift_USERS WHERE userID = (SELECT offerer_id FROM AberLift_OFFERS WHERE offer_id='$offerID')";	
	if (!mysqli_query($con,$SQL))
	{
		echo("Error description: " . mysqli_error($con));
	}
	$Result = mysqli_query($con,$SQL);
	$row = mysqli_fetch_array($Result);

	$offererName = $row['firstName'] . ' ' .  $row['lastName'];
	$offererPhone = $row['phoneNumber'];
	$offererEmail = $row['emailAddress'];

	// mail requester

	$Text = "<div style='color:rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;font-size:14px;'><div style='font-size:24px;'>Hi $requesterName,</div><div style='margin-top:10px;'>We've just sent a text to $offererName that you will be joining them from $fromName to $toName" . ". Please find the relevent details below.</div><div style='margin-top:25px;margin-bottom:20px;background-color:#2196F3;color:white;font-weight:500;font-size:16px;text-align:center;padding:5px;height:16px;line-height:16px;'>OFFERER DETAILS</div><b>Name: </b>$offererName<br><b>Phone: </b>$offererPhone<br><b>Email Address: </b>$offererEmail<div style='margin-top:25px;margin-bottom:20px;background-color:#2196F3 ;color:white;font-weight:500;font-size:15px;text-align:center;padding:4px;height:16px;line-height:16px;'>TRIP DETAILS</div><b>Boarding Point:</b> $fromName<br><b>Destination:</b> $toName<br><b>Date:</b> $formattedDate<br><b>Time:</b> $formattedTime<br><br><div style='color:#2196F3;'>The AberLift Team</div></div><div style='font-size:12px;color:rgba(0,0,0,0.30);margin-top:25px;line-height:18px;-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;'>This is an automatically generated mail. Please do not reply. For support, please write to us at <a href='mailto:support@aberlift.com'>support@aberlift.com</a>. This email was sent to $requesterEmail. Please add no-reply@aberlift.com to your contacts so that our emails can reach to you without any troubles in the future.</div>";

	$Header = "From: AberLift <no-reply@aberlift.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	mail($requesterEmail,"AberLift - Request Confirmation",$Text,$Header);
	
	// message offerer

    $user = "lifterapp";
    $password = "dGRBeZTAXCFSWP";
    $api_id = "3524842";
    $baseurl ="http://api.clickatell.com";

    $text = urlencode("Hi $offererName,\nYour offer for lift from $fromName to $toName on $formattedDate has been accepted by $requesterName" . " (" . $requesterPhone . ")" . "\nPlease check your mail for details \n- The AberLift Team");
    $todriver = $offererPhone;
    $url = "$baseurl/http/sendmsg?user=$user&password=$password&api_id=$api_id&to=$todriver&text=$text";
	@file_get_contents($url, false);

	// mail offerer

	$Text = "<div style='color:rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;font-size:14px;'><div style='font-size:24px;'>Hi $offererName,</div><div style='margin-top:10px;'>Your offer has been accepted! $requesterName will be joining in with you from $fromName to $toName" . ". Please find the relevent details below.</div><div style='margin-top:25px;margin-bottom:20px;background-color:#2196F3;color:white;font-weight:500;font-size:16px;text-align:center;padding:5px;height:16px;line-height:16px;'>REQUESTER DETAILS</div><b>Name: </b>$requesterName<br><b>Phone: </b>$requesterPhone<br><b>Email Address: </b>$requesterEmail<div style='margin-top:25px;margin-bottom:20px;background-color:#2196F3 ;color:white;font-weight:500;font-size:15px;text-align:center;padding:4px;height:16px;line-height:16px;'>TRIP DETAILS</div><b>Boarding Point:</b> $fromName<br><b>Destination:</b> $toName<br><b>Date:</b> $formattedDate<br><b>Time:</b> $formattedTime<br><br><div style='color:#2196F3;'>The AberLift Team</div></div><div style='font-size:12px;color:rgba(0,0,0,0.30);margin-top:25px;line-height:18px;-webkit-font-smoothing: antialiased;font-family:Roboto, Helvetica, Segoe UI, Arial ;'>This is an automatically generated mail. Please do not reply. For support, please write to us at <a href='mailto:support@aberlift.com'>support@aberlift.com</a>. This email was sent to $offererEmail. Please add no-reply@aberlift.com to your contacts so that our emails can reach to you without any troubles in the future.</div>";

	$Header = "From: AberLift <no-reply@aberlift.com>\r\n";
	$Header .= "MIME-Version: 1.0\r\n";
	$Header .= "Content-Type: text/html; charset=ISO-8859-1\r\n";

	mail($offererEmail,"AberLift - Offer Accepted",$Text,$Header);

	// echo reply
	echo "<img src='Resources/svg/check.svg' style='min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;'></div><div style='margin-left: 100px;margin-right: 100px;margin-top: 10px;color: rgba(0,0,0,0.87);-webkit-font-smoothing: antialiased;line-height: 18px;font-size: 14px;'><div style='text-align:center;font-size:18px;line-height:32px' class='accent'>Yay! Your trip from $fromName to $toName has been confirmed.</div><br>We'll shortly mail you the details of your trip and the offerer. We would also notify $offererName that you have accepted the offer.</div><div onclick='cancelOffer()' class='header-button' style='display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;'>close</div>";
?>