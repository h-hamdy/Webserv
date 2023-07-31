<?php
// while(1);
if ($_SERVER['REQUEST_METHOD'] == 'POST') {
  // Check if file was uploaded
  if (isset($_FILES['upload'])) {
    // Check for file upload error
    if ($_FILES['upload']['error'] != UPLOAD_ERR_OK) {
      die('Upload failed with error code ' . $_FILES['upload']['error']);
    }

    // Move uploaded file to a permanent location
    $filename = $_FILES['upload']['name'];
    $tmpname = $_FILES['upload']['tmp_name'];
    $destination = './' . $filename;
    if (move_uploaded_file ($tmpname ,$destination )) {
      echo 'File uploaded successfully!';
    } else {
      echo 'Error while uploading file';
    }

  } else {
    // Output message if no file was uploaded
    echo 'No file uploaded.';
  }
}
?>

<!-- HTML form to upload files -->
<form action="index.php" method="post" enctype="multipart/form-data">
  <input type="file" name="upload">
  <input type="submit" value="Upload">
</form>