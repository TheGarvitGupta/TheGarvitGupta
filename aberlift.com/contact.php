<?php
	session_start();
?>
<meta charset="UTF-8">
<html>
<head>
	<link href="Resources/css/home.css" rel="stylesheet" type="text/css">
	<link href="Resources/css/spinner.css" rel="stylesheet" type="text/css">
	<script src='Resources/JS/jquery.min.js'></script> 
	<script src="Resources/JS/jquery.maskedinput.min.js" type="text/javascript"></script>

	<style type="text/css">
		.box-center
		{
			margin-left:auto;
			margin-right:auto;
		}
		.about-page-container
		{
			width:1000px;
		}
		.about-headline
		{
			font-size: 24px;
			font-weight: 400;
			line-height: 32px;
			margin-bottom: 30px;
			overflow: hidden;
			padding-top: 145px;
			font-family: Roboto, Helvetica;
			width:1000px;

			color:#2196F3;
		}

		.about-text-container
		{
			font-size:16px;
			line-height:24px;
			width:1000px;

			color: rgb(0,0,0);
		}


		.sign-content
		{
			width:100%;
			padding-top:50px;
			padding-bottom:50px;
		}
		.sub-sign-content
		{
			width:400px;
		}
		.center
		{
			margin-left:auto;
			margin-right:auto;
			display: block;
		}
		.button
		{

		}
		.sign-in-objects
		{
			margin-top:25px;
		}
		.accent
		{
			color:#2196F3;
		}
		#button-container
		{
			height:32px;
		}
		textarea
		{
			resize:vertical;
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
		display:none;

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
	<script>
		function Send()
		{
			var name = document.getElementById("name").value;
			var subject = document.getElementById("subject").value;
			var email = document.getElementById("email").value;
			var message = document.getElementById("message").value;

			document.getElementById("button-container").innerHTML = "<svg class='center spinner' style='margin-top:50px' width='32px' height='32px' viewBox='0 0 66 66' xmlns='http://www.w3.org/2000/svg'><circle class='path' fill='none' stroke-width='6' stroke-linecap='round' cx='33' cy='33' r='30'></circle></svg>";
		
			xmlhttp = new XMLHttpRequest();
			xmlhttp.onreadystatechange = function () {
				if (xmlhttp.readyState == 4 && xmlhttp.status == 200) {
					var Reply = xmlhttp.responseText;
					if (Reply == '1')
					{	
						document.getElementById("message-window").innerHTML = '<img src="Resources/svg/sent.svg" style="min-width:200px;min-height:200px;margin-top:25px;margin-left:300px;margin-right:300px;"><div style="margin-left:100px;margin-right:100px;margin-top:10px;color:rgba(0,0,0,0.87);-webkit-font-smoothing:antialiased;line-height:18px;font-size:14px"><div style="text-align:center;font-size:18px;line-height:32px"class="accent">Message Received</div><br>Thanks, ' + name + '. We have received your message. We will review it and will get back to you if required.</div><div onclick="closeMessage()" class="header-button" style="display:block;width:60px;margin-left:auto;margin-right:auto;margin-top:40px;">close</div>';
						document.getElementById("messageWindow-full").style.display = "block";
						
						setTimeout(function(){
							document.getElementById("messageWindow-full").style.opacity = "1";
							document.getElementById("message-window").style.boxShadow = "rgba(0, 0, 0, 0.298039) 0px 19px 38px, rgba(0, 0, 0, 0.219608) 0px 15px 12px";
							document.getElementById("button-container").innerHTML = "<div onclick='Send()' class='center header-button' style='width:125px;margin-top:50px'>Send Message</div>";
						}, 250);
					}
					else
					{
						alert(Reply);
					}
				}
			}
			xmlhttp.open("POST","contactQuery.php",true);
			xmlhttp.setRequestHeader("Content-type","application/x-www-form-urlencoded");
			xmlhttp.send("name=" + name  + "&subject=" + subject + "&email=" + email + "&message=" + message);
		}

		function closeMessage()
		{
			document.getElementById("message-window").style.boxShadow = "0px 0px 0px rgb(0,0,0)";
			document.getElementById("messageWindow-full").style.opacity = "0";
			setTimeout(function(){ 	document.getElementById("message-window").innerHTML = ""; document.getElementById("messageWindow-full").style.display = "none"; }, 500);
		}
	</script>
	<script type="text/javascript">
		$(document).ready(function() {
			$('#name').keypress(function (e) {
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
			$('#name', '#subject', '#email').keypress(function() {
			    if($(this).val().length >= 100) {
			        $(this).val($(this).val().slice(0, 100));
			        return false;
			    }
			});
		});

		$(document).ready(function() {
			$('#message').keypress(function() {
			    if($(this).val().length >= 1000) {
			        $(this).val($(this).val().slice(0, 1000));
			        return false;
			    }
			});
		});		
	</script>
</head>
<body>
	<?php include 'header.php' ?>

	<div id="messageWindow-full">
		<div id="message-window"></div>
	</div>


	<div class="box-center about-page-container">
		<div class="about-headline">
			Contact Us
		</div>

		<div class="sign-content">
			<div class="center sub-sign-content">
				<input id="name" type="text" class="object center sign-in-objects" style="width:400px" placeholder="Full Name"></input>
				<input id="subject" type="text" class="object center sign-in-objects" style="width:400px" placeholder="Subject"></input>
				<input id="email" type="text" class="object center sign-in-objects" style="width:400px" placeholder="Email Address"></input>
				<textarea id="message" class="object center sign-in-objects" style="width:400px" placeholder="Message"></textarea>

				<div id="button-container">
					<div onclick='Send()' class='center header-button' style='width:125px;margin-top:50px'>Send Message</div>
				</div>
			</div>
		</div>

	</div>

	<?php include 'footer.php' ?>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
</body>
</html>