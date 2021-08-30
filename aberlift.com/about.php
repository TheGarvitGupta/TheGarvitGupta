<?php
	session_start();
?>
<meta charset="UTF-8">
<html>
<head>
	<link href="Resources/css/home.css?s" rel="stylesheet" type="text/css">
	<script src='Resources/JS/jquery.min.js'></script> 
	<script src="Resources/JS/home.js?s"></script>

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
		.about-sub-headline
		{
			font-size:16px;
			line-height:24px;
			color:rgb(0,0,0);
			margin-top:100px;
			padding-top:100px;
			border-top:solid 1px rgba(0,0,0,0.12);
		}
		.team-members-container
		{
			width:1000px;
			height:400px;
		}
		.team-acknowledgements-container
		{
			padding-top:30px;
			padding-bottom:70px;
			margin-bottom:100px;
			border-bottom:solid 1px rgba(0,0,0,0.12);
			width:1000px;
		}
		.about-text-container
		{
			font-size:16px;
			line-height:24px;
			width:1000px;

			color: rgb(0,0,0);
		}

		.team-member-frame
		{
			position:relative;
			width:300px;
			height:300px;
			padding-top:100px;
			margin-right:50px;
			float:left;
			box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
		}
		.member-picture
		{
			position:absolute;
			height:100px;
			width:100px;
			top:50px;
			left:100px;
			border-radius: 50%;
			box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
			background-color:white;

			z-index: 2;
		}
		.member-name
		{
			position:absolute;

			height:16px;
			width:300px;
			text-align:center;
			font-family:Roboto;
			font-weight:500;
			font-size:16px;
			color:rgba(0,0,0,0.7);
			top:170px;
			line-height:16px;
			text-transform: uppercase;
		}
		.team-area-divider
		{
			height:1px;
			width:300px;
			background-color:rgba(0,0,0,0.12);

			position:absolute;
			top:100px;
			left:0px;

			z-index: 1;
		}
		.member-location-container
		{
			position:absolute;
			top:190px;
			text-align:center;
			width:300px;
			height:15px;
			opacity:0.3;
		}
		.member-location
		{
			display:inline-block;
			font-size:14px;
			line-height:14px;

			padding-left:14px;
			background-image:url('http://beta.garvitgupta.com/Resources/svg/location.svg');
			background-size:14px;
			background-position:0px 0px;
			background-repeat: no-repeat;
		}
		.member-text
		{
			position:absolute;
			top:205px;
			left:0px;

			color:rgba(0,0,0,0.7);
			font-size:14px;
			line-height: 20px;


			padding-left:25px;
			padding-right:25px;
			padding-top:15px;


			height:120px;
			width:250px;

			overflow:hidden;
		}

		.person-cover
		{
			background-size:contain;
			background-position:0px 0px;
			background-repeat: no-repeat;
			background-image:url("http://beta.garvitgupta.com/Resources/images/14%20-%206.png");
		}

		.person-picture
		{
			background-size:cover;
			background-position:center;
			background-repeat: no-repeat;
			background-image:url("http://images5.fanpop.com/image/photos/24900000/NC-neal-caffrey-24912911-500-281.png");
		}


		.acknowledgement-frame
		{
			width:1000px;
			padding-top:25px;
			padding-bottom:25px;
			background-color:white;
			min-height:50px;
			line-height: 25px;

			color:rgba(0,0,0,0.7);
		}
		.ack-person-details
		{
			float:left;
			height:50px;
			margin-right:50px;
			margin-bottom:1px;
		}
		.ack-person-picture
		{
			box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
			height:50px;
			width:50px;
			border-radius:50%;
			float:left;
			margin-right:10px;

			background-size:50px;
			background-position: center;
		}
		.ack-person-header
		{
			height:50px;
			float:left;
		}
		.ack-person-name
		{
			line-height:25px;
		}
		.ack-person-location
		{
			line-height:25px;
			color:rgba(0,0,0,0.5);
			font-size:14px;
		}


		.flat-button
		{
			cursor:pointer;

			height:32px;
			width:32px;
			background-color:white;

			-webkit-transition: all 250ms;
			moz-transition: all 250ms;

			-webkit-filter: grayscale(100%);
			filter: grayscale(100%);
		}
		.flat-button:hover
		{
			box-shadow: 0 1px 3px 0 rgba(0, 0, 0, 0.12), 0 1px 2px 0 rgba(0, 0, 0, 0.24);
		}
		.flat-button:active
		{
			box-shadow: 0 3px 6px 0 rgba(0, 0, 0, 0.16), 0 3px 6px 0 rgba(0, 0, 0, 0.23);
		}
		.facebook
		{
			position: absolute;
			bottom:10px;
			left:25px;

			background-image: url('Resources/images/social_click/facebook.png');
			background-repeat: no-repeat;
			background-position:center;
		}
		.twitter
		{
			position: absolute;
			bottom:10px;
			left:73px;

			background-image: url('Resources/images/social_click/twitter.png');
			background-repeat: no-repeat;
			background-position:center;
		}
		.mail
		{
			position: absolute;
			bottom:10px;
			left:121px;

			background-image: url('Resources/images/social_click/mail.png');
			background-repeat: no-repeat;
			background-position:center;
		}
		.website
		{
			position: absolute;
			bottom:10px;
			left:169px;

			background-image: url('Resources/images/social_click/website.png');
			background-repeat: no-repeat;
			background-position:center;
		}

		.div_facebook
		{
			position:absolute;
			bottom:18px;
			background-color:rgba(0,0,0,0.12);
			left:65px;

			height:16px;
			width:1px;
		}
		.div_twitter
		{
			position:absolute;
			bottom:18px;
			background-color:rgba(0,0,0,0.12);
			left:113px;

			height:16px;
			width:1px;
		}
		.div_mail
		{
			position:absolute;
			bottom:18px;
			background-color:rgba(0,0,0,0.12);
			left:161px;

			height:16px;
			width:1px;
		}
		.div_website
		{
			position:absolute;
			bottom:18px;
			background-color:rgba(0,0,0,0.12);
			left:209px;

			height:16px;
			width:1px;
		}



	</style>
</head>
<body>
	<?php include 'header.php' ?>

	<div class="box-center about-page-container">
		<div class="about-headline">
			AberLift Team
		</div>

		<div class="team-members-container">
			<div class="team-member-frame person-cover">
				<div class="team-area-divider"></div>
				<div class="member-picture person-picture"></div>
				<div class="member-name">John Doe</div>
				<div class="member-location-container">
					<div class="member-location">The Place</div>
				</div>
				<div class="member-text">
					Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque tincidunt justo sit amet risus efficitur, quis ultrices sem blandit. Proin non lorem in massa congue tempor. Duis ac nulla a nunc dictum convallis. Quisque maximus malesuada sapien, eget tristique justo consectetur quis. Pellentesque sed tincidunt nisl. Sed laoreet sapien quis sapien dictum ultricies. Quisque id libero urna.
				</div>
				<div class="flat-button facebook"></div>
				<div class="flat-button twitter"></div>
				<div class="flat-button mail"></div>
				<div class="flat-button website"></div>
				<div class="div_facebook"></div>
				<div class="div_twitter"></div>
				<div class="div_mail"></div>
			</div>

			<div class="team-member-frame person-cover">
				<div class="team-area-divider"></div>
				<div class="member-picture person-picture"></div>
				<div class="member-name">John Doe</div>
				<div class="member-location-container">
					<div class="member-location">The Place</div>
				</div>
				<div class="member-text">
					Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque tincidunt justo sit amet risus efficitur, quis ultrices sem blandit. Proin non lorem in massa congue tempor. Duis ac nulla a nunc dictum convallis. Quisque maximus malesuada sapien, eget tristique justo consectetur quis. Pellentesque sed tincidunt nisl. Sed laoreet sapien quis sapien dictum ultricies. Quisque id libero urna.
				</div>
				<div class="flat-button facebook"></div>
				<div class="flat-button twitter"></div>
				<div class="flat-button mail"></div>
				<div class="flat-button website"></div>
				<div class="div_facebook"></div>
				<div class="div_twitter"></div>
				<div class="div_mail"></div>
			</div>

			<div class="team-member-frame person-cover" style="margin-right:0px;">
				<div class="team-area-divider"></div>
				<div class="member-picture person-picture"></div>
				<div class="member-name">John Doe</div>
				<div class="member-location-container">
					<div class="member-location">The Place</div>
				</div>
				<div class="member-text">
					Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque tincidunt justo sit amet risus efficitur, quis ultrices sem blandit. Proin non lorem in massa congue tempor. Duis ac nulla a nunc dictum convallis. Quisque maximus malesuada sapien, eget tristique justo consectetur quis. Pellentesque sed tincidunt nisl. Sed laoreet sapien quis sapien dictum ultricies. Quisque id libero urna.
				</div>
				<div class="flat-button facebook"></div>
				<div class="flat-button twitter"></div>
				<div class="flat-button mail"></div>
				<div class="flat-button website"></div>
				<div class="div_facebook"></div>
				<div class="div_twitter"></div>
				<div class="div_mail"></div>
			</div>
		</div>

		<div class="about-sub-headline">
			We are grateful to the amazing people who have helped us most throughout the development of AberLift. Our sincere thanks and regards for their time and efforts for this project:
		</div>

		<div class="team-acknowledgements-container">
			<div class="acknowledgement-frame">
				<div class="ack-person-details">
					<div class="ack-person-picture" style="background-image:url('http://upload.wikimedia.org/wikipedia/commons/0/01/Bill_Gates_July_2014.jpg');"></div>
					<div class="ack-person-header">
						<div class="ack-person-name">Bill Gates</div>
						<div class="ack-person-location">Microsoft</div>
					</div>
				</div>
				We are grateful to the amazing people who have helped us most throughout the development of AberLift. Our sincere thanks and regards for their time and efforts for this project:	
			</div>
			<div class="acknowledgement-frame">
				<div class="ack-person-details">
					<div class="ack-person-picture" style="background-image:url('http://upload.wikimedia.org/wikipedia/commons/thumb/5/51/Warren_Buffett_KU_Visit.jpg/220px-Warren_Buffett_KU_Visit.jpg');"></div>
					<div class="ack-person-header">
						<div class="ack-person-name">Warren Buffett</div>
						<div class="ack-person-location">Berkshire Hathaway</div>
					</div>
				</div>
				We are grateful to the amazing people who have helped us most throughout the development of AberLift. Our sincere thanks and regards for their time and efforts for this project:	
			</div>
			<div class="acknowledgement-frame">
				<div class="ack-person-details">
					<div class="ack-person-picture" style="background-image:url('http://upload.wikimedia.org/wikipedia/commons/thumb/b/b9/Steve_Jobs_Headshot_2010-CROP.jpg/220px-Steve_Jobs_Headshot_2010-CROP.jpg');"></div>
					<div class="ack-person-header">
						<div class="ack-person-name">Steve Jobs</div>
						<div class="ack-person-location">Apple</div>
					</div>
				</div>
				We are grateful to the amazing people who have helped us most throughout the development of AberLift. Our sincere thanks and regards for their time and efforts for this project:	
			</div>
			<div class="acknowledgement-frame">
				<div class="ack-person-details">
					<div class="ack-person-picture" style="background-image:url('http://upload.wikimedia.org/wikipedia/commons/e/eb/Stephen_Hawking.StarChild.jpg');"></div>
					<div class="ack-person-header">
						<div class="ack-person-name">Stephen Hawking</div>
						<div class="ack-person-location">Theoretical Physicist</div>
					</div>
				</div>
				We are grateful to the amazing people who have helped us most throughout the development of AberLift. Our sincere thanks and regards for their time and efforts for this project:	
			</div>
		</div>

		<div class="about-headline" style="padding-top:0px;">
			About
		</div>

		<div class="about-text-container">
			Lorem ipsum dolor sit amet, consectetur adipiscing elit. Pellentesque tincidunt justo sit amet risus efficitur, quis ultrices sem blandit. Proin non lorem in massa congue tempor. Duis ac nulla a nunc dictum convallis. Quisque maximus malesuada sapien, eget tristique justo consectetur quis. Pellentesque sed tincidunt nisl. Sed laoreet sapien quis sapien dictum ultricies. Quisque id libero urna.

			Praesent vitae dolor sed urna mollis vulputate. Ut sed sagittis arcu, vitae aliquet leo. Suspendisse porttitor est ex, id placerat augue ultricies ut. Vestibulum risus elit, imperdiet faucibus commodo nec, ornare eu mauris. Integer id urna velit. Fusce egestas vitae massa vel malesuada. Praesent vulputate erat in consectetur molestie. Mauris ac magna id nibh dignissim rhoncus sit amet at arcu.

			Vivamus massa ipsum, eleifend quis augue at, egestas tincidunt lectus. Nunc finibus in odio dapibus luctus. Vestibulum hendrerit ultricies gravida. Duis pellentesque, massa ut tincidunt rutrum, sem est ornare lorem, in sodales enim odio in orci. Nam maximus nibh interdum ipsum tempus, et viverra libero scelerisque. Aenean eu urna eget ipsum gravida cursus et nec eros. Vestibulum ante ipsum primis in faucibus orci luctus et ultrices posuere cubilia Curae; Sed in fermentum magna. Aenean non tempor libero. Maecenas ullamcorper imperdiet cursus. Duis scelerisque enim nunc, vitae feugiat quam mollis a. Integer eget tellus sit amet quam elementum gravida. Nam vel orci et nisl egestas sagittis consectetur vitae nunc. Mauris ac odio eu tortor tristique pretium.

			Nunc eu sollicitudin orci. Quisque vel laoreet odio. Suspendisse et mi dictum, fringilla leo eu, accumsan tellus. Integer vitae egestas tortor, id vestibulum lacus. Lorem ipsum dolor sit amet, consectetur adipiscing elit. Cras condimentum semper neque tempor fringilla. Aliquam et libero ut turpis iaculis porta. Sed nec tincidunt tortor. Fusce vel vulputate neque, nec posuere leo. Aenean id scelerisque urna. Etiam porta ex et mauris vestibulum, vitae eleifend quam ultricies.

			Pellentesque pellentesque nunc non augue posuere malesuada a at nisl. Duis lacinia risus ut luctus feugiat. Maecenas porttitor placerat mattis. Integer hendrerit enim a est semper eleifend. Nulla et pulvinar lorem. Quisque vel hendrerit nunc. Fusce venenatis porttitor tincidunt. Nunc in turpis maximus, pulvinar enim ut, molestie arcu. Sed porta quis purus ac dapibus. Aliquam non purus eget urna fringilla posuere. Proin a vulputate ipsum, at malesuada erat.
		</div>
	</div>

	<?php include 'footer.php' ?>
	<link href='http://fonts.googleapis.com/css?family=Roboto:400,300,100' rel='stylesheet' type='text/css'>
</body>
</html>