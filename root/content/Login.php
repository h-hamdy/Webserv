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
                $_COOKIE['email'] = $_POST['email'];
                $_COOKIE['avatar'] = $avatar_destination;

                // Set cookie expiration time based on "Remember me" checkbox
                $cookie_expiration = isset($_POST['remember_me']) ? time() + 3600 * 24 * 7 : time() + 3600 * 24;

                setcookie('name', $_POST['name'], $cookie_expiration);
                setcookie('email', $_POST['email'], $cookie_expiration);
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
    unset($_COOKIE['email']);
    unset($_COOKIE['avatar']);
    setcookie('name', '', time() - 3600);
    setcookie('email', '', time() - 3600);
    setcookie('avatar', '', time() - 3600);
}
?>
<!DOCTYPE html>
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
</style>