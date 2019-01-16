jQuery(window).load(function () {
	
	$(".cover" ).css("opacity","0");
	$(".name" ).css("text-shadow","5px 5px 25px rgba(0,0,0,0.35)");
	$(".designation" ).css("text-shadow","5px 5px 25px rgba(0,0,0,0.35)");

	/* Shouldn't make the user wait for 1500 seconds to scroll. Let them scroll earlier if they are desparate */
	setTimeout(
		function(){
			enableScroll();
		}, 500	
	);

	setTimeout(
		function(){
			$(".nav-dot").eq(0).addClass("animated fadeInRight");
		}, 500
	);
	setTimeout(
		function(){
			$(".nav-dot").eq(1).addClass("animated fadeInRight");
		}, 700
	);
	setTimeout(
		function(){
			$(".nav-dot").eq(2).addClass("animated fadeInRight");
		}, 900
	);
	setTimeout(
		function(){
			$(".nav-dot").eq(3).addClass("animated fadeInRight");
		}, 1100
	);
	
	
	setTimeout(
		function(){
			$(".cover" ).css("display","none");
			$(".down-arrow" ).css("cursor","pointer");
			$(".down-button" ).css("cursor","pointer");
		}, 1500	
	);			
});

/* prefetch images */

Image2= new Image(48,50);
Image2.src = "images/icons/contact/app-blue.png";

Image3= new Image(48,50);
Image3.src = "images/icons/contact/job-blue.png";

Image4= new Image(48,50);
Image4.src = "images/icons/contact/mail-blue.png";

Image5= new Image(48,50);
Image5.src = "images/icons/contact/web-blue.png";		
