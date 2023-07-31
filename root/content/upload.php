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
    $destination = '../uploads/' . $filename;
    // check if destination exists if not create it
    if (!file_exists('../uploads/')) {
      mkdir('../uploads/', 0777, true);
    }
    if (move_uploaded_file ($tmpname ,$destination )) {
      echo 'File uploaded successfully!';
      exit();
    } else {
      echo 'Error while uploading file';
      exit();
    }

  } else {
    // Output message if no file was uploaded
    echo 'No file uploaded.';
    exit();
  }
}
?>

<!DOCTYPE html>
<html lang="en">
<head>
<meta charset="UTF-8">
<title>VibeSphere - Upload</title>
<link rel="stylesheet" href="styles.css">
</head>
<body>
<header>
  <div><h1>The VibeSphere Server</h1></div>
  <div class="Pdi"><p>Experience the Journey</p></div>
</header>

<center>
<div class="content">
  <div class="login-box">
    <div class="user-box">
    <form action="upload.php" method="post" enctype="multipart/form-data">
      <input class="upload_button" type="file" name="upload" required>
      <a href="#">
        <span></span>
        <span></span>
        <span></span>
        <span></span>
      <button type="submit" value="Upload">Upload</button>
      </a>
    </form>
    </div>
  </div>
</div>
</center>

<footer>
  <p>&copy; 2023 VibeSphere Server. All rights reserved.</p>
</footer>
</body>
</html>


<style>
body {
    font-family: Arial, sans-serif;
    margin:0;
    padding:0;
    font-family: sans-serif;
    background: linear-gradient(#141e30, #243b55);
}

header {
    color: #fff;
	  align-content: center;
    align-items: center;
    background-color: #2c3540;
    display: flex;
    flex: none;
    flex-direction: column;
    flex-wrap: nowrap;
    gap: 10px;
    height: min-content;
    justify-content: center;
    overflow: hidden;
    padding: 100px 50px;
    position: relative;
    width: 100%;;
}


header h1 {
    margin: 0;
}

h1 {
	font-family: "Bowlby One", Inter, Inter Placeholder, sans-serif;
	font-weight: 400;
	color: #ffffff;
	font-size: 73.28px;
	letter-spacing: 0;
	text-transform: none;
	text-decoration: none;
	line-height: 1.1;
	text-align: center;
}

h3 {
  font-family: "Bowlby One", Inter, Inter Placeholder, sans-serif;
	font-weight: 400;
	color: #ffffff;
	font-size: 35px;
  text-shadow: 0 0 10px #03e9f4;
	letter-spacing: 0;
	text-transform: none;
	text-decoration: none;
	line-height: 1.1;
	text-align: center;
}

p {
	font-family: "DM Sans", Inter, Inter Placeholder, sans-serif;
	font-style: normal;
	font-weight: 500;
	color: #ffffff;
	font-size: 16px;
	letter-spacing: 0;
	text-transform: none;
	text-decoration: none;
	line-height: 1.2em;
}

main {
    padding: 20px;
}

.content {
	color: #fff;
	align-content: center;
  align-items: center;
  background-color: #465973;
  display: flex;
  flex: none;
  flex-direction: row;
  flex-wrap: nowrap;
  gap: 10px;
  height: min-content;
  justify-content: center;
  overflow: hidden;
  padding: 100px 50px;
  position: relative;
  width: 100%;;
  height: 60vh;
}

button {
  position: relative;
  display: inline-block;
  padding: 5px 30px;
  color: #fff;
  text-decoration: none;
  text-transform: uppercase;
  letter-spacing: 2px;
  border-radius: 7px;
  background-color: transparent;
  overflow: hidden;
  transition: 0.5s;
  border: 2px solid #03e9f4;
}

.upload_button {
  position: relative;
  display: inline-block;
  padding: 5px 30px;
  color: #fff;
  text-decoration: none;
  border-radius: 7px;
  background-color: transparent;
  overflow: hidden;
  transition: 0.5s;
  border: 2px solid #03e9f4;
}

footer {
  color: #fff;
  align-content: center;
  align-items: center;
  background-color: #2c3540;
  display: flex;
  flex: none;
  flex-direction: row;
  flex-wrap: nowrap;
  gap: 20px;
  height: min-content;
  justify-content: center;
  overflow: hidden;
  padding: 100px;
  position: relative;
  width: 100%;
}

footer p {
    margin: 0;
}

.login-box {
  position: absolute;
  top: 50%;
  left: 50%;
  width: 400px;
  padding: 40px;
  transform: translate(-50%, -50%);
  background: rgba(0,0,0,.5);
  box-sizing: border-box;
  box-shadow: 0 15px 25px rgba(0,0,0,.6);
  border-radius: 10px;
}

.login-box h2 {
  margin: 0 0 30px;
  padding: 0;
  color: #fff;
  text-align: center;
}

.login-box .user-box {
  position: relative;
}

.login-box .user-box input {
  width: 100%;
  padding: 10px 0;
  font-size: 16px;
  color: #fff;
  margin-bottom: 30px;
  border: none;
  border-bottom: 1px solid #fff;
  outline: none;
  background: transparent;
}
.login-box .user-box label {
  position: absolute;
  top:0;
  left: 0;
  padding: 10px 0;
  font-size: 16px;
  color: #fff;
  pointer-events: none;
  transition: .5s;
}

.login-box .user-box input:focus ~ label,
.login-box .user-box input:valid ~ label {
  top: -20px;
  left: 0;
  color: #03e9f4;
  font-size: 12px;
}

.login-box form a {
  position: relative;
  display: inline-block;
  padding: 10px 20px;
  color: #03e9f4;
  font-size: 16px;
  text-decoration: none;
  text-transform: uppercase;
  overflow: hidden;
  transition: .5s;
  letter-spacing: 4px
}

.login-box a:hover {
  background: #03e9f4;
  color: #fff;
  border-radius: 5px;
  box-shadow: 0 0 5px #03e9f4,
              0 0 25px #03e9f4,
              0 0 50px #03e9f4,
              0 0 100px #03e9f4;
}

.login-box a span {
  position: absolute;
  display: block;
}

.login-box a span:nth-child(1) {
  top: 0;
  left: -100%;
  width: 100%;
  height: 2px;
  background: linear-gradient(90deg, transparent, #03e9f4);
  animation: btn-anim1 1s linear infinite;
}

@keyframes btn-anim1 {
  0% {
    left: -100%;
  }
  50%,100% {
    left: 100%;
  }
}

.login-box a span:nth-child(2) {
  top: -100%;
  right: 0;
  width: 2px;
  height: 100%;
  background: linear-gradient(180deg, transparent, #03e9f4);
  animation: btn-anim2 1s linear infinite;
  animation-delay: .25s
}

@keyframes btn-anim2 {
  0% {
    top: -100%;
  }
  50%,100% {
    top: 100%;
  }
}

.login-box a span:nth-child(3) {
  bottom: 0;
  right: -100%;
  width: 100%;
  height: 2px;
  background: linear-gradient(270deg, transparent, #03e9f4);
  animation: btn-anim3 1s linear infinite;
  animation-delay: .5s
}

@keyframes btn-anim3 {
  0% {
    right: -100%;
  }
  50%,100% {
    right: 100%;
  }
}

.login-box a span:nth-child(4) {
  bottom: -100%;
  left: 0;
  width: 2px;
  height: 100%;
  background: linear-gradient(360deg, transparent, #03e9f4);
  animation: btn-anim4 1s linear infinite;
  animation-delay: .75s
}

@keyframes btn-anim4 {
  0% {
    bottom: -100%;
  }
  50%,100% {
    bottom: 100%;
  }
}


.wrapper{
  height: 400px;
  width: 320px;
  position: relative;
  transform-style: preserve-3d;
 perspective: 1000px;
}

.wrapper .card{
  position: absolute;
  height: 100%;
  width: 100%;
  padding: 5px;
  background: rgba(0,0,0,.5);
  border-radius: 10px;
  transform: translateY(0deg);
  transform-style: preserve-3d;
  backface-visibility: hidden;
  box-shadow: 0px 10px 15px rgba(0,0,0,0.1);
  transition: transform 0.7s cubic-bezier(0.4,0.2,0.2,1);
}

.wrapper:hover > .front-face{
  transform: rotateY(-180deg);
}
.wrapper .card img{
  height: 100%;
  width: 100%;
  object-fit: cover;
  border-radius: 10px;
}
.wrapper .back-face{
  display: flex;
  align-items: center;
  justify-content: space-evenly;
  flex-direction: column;
  transform: rotateY(180deg);
}
.wrapper:hover > .back-face{
  transform: rotateY(0deg);
}
.wrapper .back-face img{
  height: 150px;
  width: 150px;
  padding: 5px;
  border-radius: 50%;
  background: linear-gradient(90deg, transparent, #03e9f4);
}
.wrapper .back-face .info{
  text-align: center;
}
.back-face .info .title{
  font-size: 30px;
  font-weight: 500;
}
.back-face ul{
  display: flex;
}
.back-face ul a{
  display: block;
  height: 40px;
  width: 40px;
  text-align: center;
  margin: 0 5px;
  line-height: 38px;
  border-radius: 50%;
  background: linear-gradient(90deg, transparent, #03e9f4);
  transition: all 0.5s ease;
}
.back-face ul a:hover{
  background: linear-gradient(90deg, transparent, #03e9f4);
}

</style>