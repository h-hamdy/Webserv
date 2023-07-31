<?php

if ($_SERVER['REQUEST_METHOD'] == 'POST')
{

    // save $_FILES['avatar'] to a inside a folder
    $avatar = $_FILES['avatar'];
    $avatar_name = $avatar['name'];
    $avatar_tmp_name = $avatar['tmp_name'];
    $avatar_size = $avatar['size'];
    $avatar_error = $avatar['error'];
    //while(1);
    $avatar_ext = explode('.', $avatar_name);
    $avatar_actual_ext = strtolower(end($avatar_ext));

    $allowed = array('jpg', 'jpeg', 'png', 'svg');
    if (in_array($avatar_actual_ext, $allowed))
    {
        if ($avatar_error === 0) 
        {
            if ($avatar_size < 1000000) 
            {
                $avatar_name_new = uniqid('', true) . '.' . $avatar_actual_ext;
                if (!file_exists("./uploadCGI/"))
                    mkdir('uploadCGI');
                $avatar_destination = './uploadCGI/' . $avatar_name_new;
                move_uploaded_file($avatar_tmp_name, $avatar_destination);
                $_COOKIE['name'] = $_POST['name'];
                $_COOKIE['login'] = $_POST['login'];
                $_COOKIE['avatar'] = $avatar_destination;

                // Set cookie expiration time based on "Remember me" checkbox
                $cookie_expiration = isset($_POST['remember_me']) ? time() + 3600 * 24 * 7 : time() + 3600 * 24;

                setcookie('name', $_POST['name'], $cookie_expiration);
                setcookie('login', $_POST['login'], $cookie_expiration);
                setcookie('avatar', $avatar_destination, $cookie_expiration);
                echo isset($_COOKIE['name']);
            } 
            else 
            {
                echo 'File too big';
                exit(1);
            }
        } 
        else 
        {
            echo 'Error';
            exit(1);
        }
    } 
    else 
    {
        echo 'Not allowed';
        exit(1);
    }
} 
else if ($_SERVER['REQUEST_METHOD'] == 'GET' && isset($_GET['logout'])) 
{
    unset($_COOKIE['name']);
<<<<<<< HEAD
    unset($_COOKIE['email']);
    unset($_COOKIE['avatar']);
    setcookie('name', '', time() - 3600);
    setcookie('email', '', time() - 3600);
=======
    unset($_COOKIE['login']);
    unset($_COOKIE['avatar']);
    setcookie('name', '', time() - 3600);
    setcookie('login', '', time() - 3600);
>>>>>>> 5be27fcef9c60c080ffa31aefd32b439b6975949
    setcookie('avatar', '', time() - 3600);
}
?>
<!DOCTYPE html>
<<<<<<< HEAD
<div>

    <div class="container">
    <?php if (isset($_COOKIE['name']) && isset($_COOKIE['email'])): ?>
            <img class="brand-logo" src=<?php echo $_COOKIE['avatar']; ?> alt="" srcset="">
            <div class="brand-title">INFORMATIONS</div>
            <p>Welcome To Our Webserv.</p>
            <p>
                <?= $_COOKIE['name'] ?>
            </p>
            <p>
                <?php echo $_COOKIE['email']; ?>
            </p>

            <a href="/Project/Project.html">

                <h5>Project</h5>
            </a>
            <a href="/Project/Webserv.html">

                <h4>Webserv</h4>
            </a>
            <a href="?logout">

                <h5>LOGOUT</h5>
            </a>
        <?php else: ?>
            <img class="brand-logo" src=<?php echo "./img/profileimage.png" ?> alt="" srcset="">
            <div class="brand-title">INFORMATIONS</div>
            <form class='inputs' method="post" enctype="multipart/form-data">
                <label for="name">NAME</label>
                <input class="input" type="text" id="name" name="name" placeholder="NAME" required>
                <label for="email">EMAIL</label>
                <input class="emilIn input" type="email" id="email" name="email" placeholder="example@test.com" required>
                <input style="margin-left: 50px;" type="file" id="avatar" name="avatar" accept="image/png, image/jpeg" required>
                <button type="submit" value="Submit">LOGIN</button>
            </form>
        </div>
    <?php endif; ?>
</div>

<style>
    @import url('https://fonts.googleapis.com/css2?family=Poppins:wght@400;900&display=swap');

    .input {
        caret-color: red;
    }

    .emilIn {
        margin-bottom: 20px;
    }

    body {
        margin: 0;
        width: 100vw;
        height: 100vh;
        background: #ecf0f3;
        display: flex;
        align-items: center;
        text-align: center;
        justify-content: center;
        place-items: center;
        overflow: hidden;
        font-family: poppins;
        background-color: black;
    }

    .container {
        position: relative;
        width: 350px;
        height: 541px;
        border-radius: 20px;
        padding: 40px;
        box-sizing: border-box;
        background: #ecf0f3;
        box-shadow: 5px 5px 5px #dd6b31, 5px 5px 20px #dd6b31;
    }

    .brand-logo {
        height: 100px;
        width: 100px;
        margin: auto;
        border-radius: 50%;
        box-sizing: border-box;
        box-shadow: 6px 6px 6px #dd6b31, -10px -10px 12px white;
    }

    .brand-title {
        margin-top: 10px;
        font-weight: 900;
        font-size: 1.8rem;
        color: #EE6315;
        letter-spacing: 1px;
    }

    .inputs {
        text-align: left;
        margin-top: 30px;
    }

    label,
    input,
    button {
        display: block;
        width: 100%;
        padding: 0;
        border: none;
        outline: none;
        box-sizing: border-box;
    }

    label {
        margin-bottom: 4px;
    }

    label:nth-of-type(2) {
        margin-top: 12px;
    }

    input::placeholder {
        color: gray;
    }

    .input {
        background: #ecf0f3;
        padding: 10px;
        padding-left: 20px;
        height: 50px;
        font-size: 14px;
        border-radius: 50px;
        box-shadow: inset 6px 6px 6px #cbced1, inset -6px -6px 6px white;
    }

    button,
    a {
        display: flex;
        color: white;
        margin-top: 20px;
        background: #EE6315;
        height: 40px;
        border-radius: 20px;
        cursor: pointer;
        font-weight: 900;
        box-shadow: 6px 6px 6px #cbced1, -6px -6px 6px white;
        transition: 0.5s;
        justify-content: center;
        align-items: center;
        text-decoration: none;
    }

    h4 {}
    h5 {}


    button:hover,
    a:hover {
        box-shadow: none;
    }


    h1 {
        position: absolute;
        top: 0;
        left: 0;
    }
=======
<html lang="en">
<head>
<meta charset="UTF-8">
<!-- <meta name="viewport" content="width=device-width, initial-scale=1.0"> -->
<title>Simple Page with Header, Content, and Footer</title>
<link rel="stylesheet" href="styles.css">
</head>
<body>
<header>
  <div><h1>The VibeSphere Server</h1></div>
  <div class="Pdi"><p>Experience the Journey</p></div>
</header>

<div class="content">
<?php if (isset($_COOKIE['name']) && isset($_COOKIE['login'])): ?>
		<img class="brand-logo" src=<?php echo $_COOKIE['avatar']; ?> alt="" srcset="">
		<div class="brand-title">INFORMATIONS</div>
		<p>Welcome To Our Webserv.</p>
		<p>
			<?= $_COOKIE['name'] ?>
		</p>
		<p>
			<?php echo $_COOKIE['login']; ?>
		</p>

		<a href="/Project/Project.html">

			<h5>Project</h5>
		</a>
		<a href="/Project/Webserv.html">

			<h4>Webserv</h4>
		</a>
		<a href="?logout">

			<h5>LOGOUT</h5>
		</a>
<?php else: ?>
    <img class="brand-logo" src=<?php echo "./img/profileimage.png" ?> alt="" srcset="">
    <div class="login-box">
    <h2>Login</h2>
    <form class='inputs' method="post" enctype="multipart/form-data">
      <div class="user-box">
        <input type="text" id="name" name="name" required>
        <label for="name">Full Name</label>
      </div>
      <div class="user-box">
        <input type="text" id="login" name="login" required>
        <label for="login">Login</label>
      </div>
      <div class="user-box">
        <input type="file" id="avatar" name="avatar" accept="image/png, image/jpeg">
        <label>Profile Picture</label>
      </div>
      <a href="#">
        <span></span>
        <span></span>
        <span></span>
        <span></span>
		<button type="submit" value="Submit">Login</button>
      </a>
    </form>
  </div>
  <?php endif; ?>
</div>



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
	text-align: left;
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
    margin-top: 40px;
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

  
>>>>>>> 5be27fcef9c60c080ffa31aefd32b439b6975949
</style>