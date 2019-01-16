var	imageLoader = document.getElementById("picture");
	imageLoader.addEventListener('change', handleImage, false);

function handleImage(e) {
    var reader = new FileReader();
    reader.onload = function (event) {
    	var url = "url(" + event.target.result + ")";
		$('.live-picture').css('background-image', url);

		/* Other Changes */
		
		$('.file-upload-box').css('border', 'none');
	}
	reader.readAsDataURL(e.target.files[0]);
}

function onImageDrag() {
	$('.file-upload-box').css('background-color', '#D0D0D0');
	$('.hint-text').css('opacity', '0');
}

function onImageDragOver() {
	$('.file-upload-box').css('background-color', '#E3E3E3');
	$('.hint-text').css('opacity', '1');
}