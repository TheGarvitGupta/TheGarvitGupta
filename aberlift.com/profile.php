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
	<link href="Resources/css/home.css" rel="stylesheet" type="text/css">
	<link href="Resources/css/spinner.css" rel="stylesheet" type="text/css">
	<script src='Resources/JS/jquery.min.js'></script>
	<script src="Resources/JS/jquery.maskedinput.min.js" type="text/javascript"></script>
	<script src='Resources/JS/home.js'></script> 
<style>
.bg-44
{
	background:url('Resources/images/+44.png');
	background-position:5px 16px;
	background-repeat: no-repeat;
}
#button-container
{
	height:32px;
}
.center
{
	margin-left:auto;
	margin-right:auto;
}
.trans
{
	transition: 250ms ease-out;
	-webkit-transition:250ms ease-out;
}
.profile-container
{
	margin-left:auto;
	margin-right:auto;
	margin-top:100px;
	width:400px;
}
.profile-picture-container
{
	margin-left:75px;
	margin-right:75px;
	height:250px;
	width:250px;
	background-color:#2196F3;
	margin-bottom:50px;
	background-position: center center;
    background-size: cover;
    box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
    display: inline-block;
}
.fake-choose-button
{
	position:relative;
	padding:0;
	top:225px;
	margin:0;
	left:100px;
	border-radius:50%;
	height:50px;
	width:50px;
	background:url('Resources/svg/plus.svg') white;
	background-position: center;
	background-repeat: no-repeat;
}
</style>
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
<script type="text/javascript">
function chooseFile()
{
	document.getElementById("uploadFile").click();
}
$(function() {
    $("#uploadFile").on("change", function()
    {
        var files = !!this.files ? this.files : [];
        if (!files.length || !window.FileReader) return; // no file selected, or no FileReader support
 
        if (/^image/.test( files[0].type)){ // only image file
            var reader = new FileReader(); // instance of the FileReader
            reader.readAsDataURL(files[0]); // read the local file
 
            reader.onloadend = function(){ // set image data as background of div
                $("#imagePreview").css("background-image", "url("+this.result+")");
            }
        }
    });
});
</script>
<script>
function submitForm()
{
	var AllGood = "true";

	document.getElementById("firstName").style.borderBottom = "2px solid #2196F3";
	document.getElementById("lastName").style.borderBottom = "2px solid #2196F3";
	document.getElementById("phone").style.borderBottom = "2px solid #2196F3";

	document.getElementById("firstName-error").innerHTML = "FIRST NAME";
	document.getElementById("lastName-error").innerHTML = "LAST NAME";
	document.getElementById("phone-error").innerHTML = "PHONE NUMBER";

	document.getElementById("firstName-error").style.color = "rgba(0,0,0,0.26)";
	document.getElementById("lastName-error").style.color = "rgba(0,0,0,0.26)";
	document.getElementById("phone-error").style.color = "rgba(0,0,0,0.26)";

	var fName = document.getElementById("firstName").value;
	var lName = document.getElementById("lastName").value;
	var phone = document.getElementById("phone").value;

	if (fName == "")
	{
		document.getElementById("firstName").style.borderBottom = "2px solid rgba(248,0,23,1)";
		document.getElementById("firstName-error").innerHTML = "Please enter your first name";
		document.getElementById("firstName-error").style.color = "rgba(248,0,23,1)";
		AllGood = "false";
	}
	if (lName == "")
	{
		document.getElementById("lastName").style.borderBottom = "2px solid rgba(248,0,23,1)";
		document.getElementById("lastName-error").innerHTML = "Please enter your last name";
		document.getElementById("lastName-error").style.color = "rgba(248,0,23,1)";
		AllGood = "false";
	}
	if (phone == "")
	{
		document.getElementById("phone").style.borderBottom = "2px solid rgba(248,0,23,1)";
		document.getElementById("phone-error").innerHTML = "Please enter your phone number";
		document.getElementById("phone-error").style.color = "rgba(248,0,23,1)";		
		AllGood = "false";
	}

	if (AllGood == "true")
	{
		document.getElementById("submit-button").click();
	}
}

	$(document).ready(function() {
		$('#firstName').keypress(function (e) {
		    var regex = new RegExp("^[a-zA-Z_ ]*$");
		    var str = String.fromCharCode(!e.charCode ? e.which : e.charCode);
		    if (regex.test(str)) {
		        return true;
		    }

		    e.preventDefault();
		    return false;
		});
	});

	$(document).ready(function() {
		$('#lastName').keypress(function (e) {
		    var regex = new RegExp("^[a-zA-Z_ ]*$");
		    var str = String.fromCharCode(!e.charCode ? e.which : e.charCode);
		    if (regex.test(str)) {
		        return true;
		    }

		    e.preventDefault();
		    return false;
		});
	});

	$(document).ready(function() {
		$('#firstName', '#lastName').keypress(function() {
		    if($(this).val().length >= 25) {
		        $(this).val($(this).val().slice(0, 25));
		        return false;
		    }
		});
	});


	$(document).ready(function() {
		$('#phone').mask("999 999 9999",{placeholder:" "});
	});

</script>
<script> /*Message Window Script */
function closeMessage()
{
	document.getElementById("message-window").style.boxShadow = "0px 0px 0px rgb(0,0,0)";
	document.getElementById("messageWindow-full").style.opacity = "0";
	setTimeout(function(){ 	document.getElementById("message-window").innerHTML = ""; document.getElementById("messageWindow-full").style.display = "none"; }, 500);
}

function showPopUp()
{
	var elementExists = document.getElementById("messageWindow-full");
	if (elementExists)
	{
		setTimeout(function(){
			document.getElementById("messageWindow-full").style.opacity = "1";
			document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
		}, 100);
	}
}
</script>
</head>
<body onload="showPopUp()"> <!--Show Popup if it exists-->
<?php
	if ($_GET["Status"] == "Error")
	{
		echo '<div id="messageWindow-full">';
		echo '	<div id="message-window">';
		echo '		<img src="Resources/svg/error.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;">';
		echo '		<div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px">';
		echo '			<div style="text-align:center;font-size:18px;line-height:32px"class="accent">Ouch! An error occured</div>';
		echo '			<br>The profile could not be updated because an error occured.';
		echo 			$_GET["Message"];
		echo '		</div>';
		echo '		<div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
		echo '	</div>';
		echo '</div>';
	}
	else if ($_GET["Status"] == "Success")
	{
		echo '<div id="messageWindow-full">';
		echo '	<div id="message-window">';
		echo '		<img src="Resources/svg/check.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;">';
		echo '		<div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px">';
		echo '			<div style="text-align:center;font-size:18px;line-height:32px"class="accent">The changes have been saved.</div>';
		echo '		</div>';
		echo '		<div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:70px;">close</div>';
		echo '	</div>';
		echo '</div>';
	}
?>

	<?php include 'header.php'; ?>

	<?php
		include 'DB_Connect.php';

		$UserID = $_SESSION["UserID"];
		$SQL = "SELECT * FROM AberLift_USERS WHERE userID = '$UserID'";
		if (!mysqli_query($con,$SQL))
		{
			echo("Error description: " . mysqli_error($con));
		}

		$Result = mysqli_query($con,$SQL);
		$row = mysqli_fetch_array($Result);

		$firstName = $_SESSION["FirstName"];
		$lastName = $_SESSION["LastName"];
		$phoneNumber = substr($_SESSION["PhoneNumber"], 2);
		$emailAddress = $_SESSION["EmailAddress"];
		$picture = 'uploads/' . $_SESSION["Picture"];

		if (strpos($picture, "?refresh=") >= 10)
		{
			$pictureName = substr($picture, 0, strpos($picture, "?refresh="));
		}
		else
		{
			$pictureName = $picture;
		}

		if (!(file_exists($pictureName)))
		{
			$picture = 'Resources/images/person_generic.png';
		}
	?>

	<div class="profile-container">
		<div id="imagePreview" class="profile-picture-container" style="background-image:url('<?php echo $picture ?>');background-position:center center;background-size: cover;">
			<div class="header-button fake-choose-button" onclick="chooseFile()"></div>
		</div>
		

		<form action="upload.php" method="post" enctype="multipart/form-data">
			<div style="height:0px;overflow:hidden">
				<input id="uploadFile" type="file" name="fileToUpload" class="img" />
			</div>

			<input id="firstName" name="fName" type="text" class="trans object center sign-in-objects" style="width:400px" placeholder="First Name" value="<?php echo $firstName ?>"></input>
			<div id="firstName-error" class="trans hint">FIRST NAME</div>

			<input id="lastName" name="lName" type="text" class="trans object center sign-in-objects" style="width:400px" placeholder="Last Name" value="<?php echo $lastName ?>"></input>
			<div id="lastName-error" class="trans hint">LAST NAME</div>

			<input id="phone" name="phone" type="text" class="trans bg-44 object center sign-in-objects" style="width:400px;padding-left:45px" placeholder="Phone Number" value="<?php echo $phoneNumber ?>"></input>
			<div id="phone-error" class="trans hint">PHONE NUMBER</div>

			<input disabled id="emailAddress" type="text" class="trans object center sign-in-objects" style="width:400px;opacity:0.75;border-bottom-color:rgb(143,143,143)" placeholder="Email Address" value="<?php echo $emailAddress ?>"></input>
			<div id="emailAddress-error" class="trans hint">EMAIL ADDRESS</div>				

			<div style="height:0px;overflow:hidden">
				<input id="submit-button" type="submit" value="Upload Image" name="submit">
			</div>
			
			<div id="button-container">
				<div onclick="submitForm()" class="center header-button" style="width:50px;margin-top:50px">Save</div>
			</div>
		</form>
	</div>

	<?php include 'footer.php';?>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>

</body>
</html>