// listen for auth status changes
auth.onAuthStateChanged(user => {
    if (user) {
        console.log("user logged in");
        console.log(user);
        setupUI(user);
        var uid = user.uid;
        console.log(uid);
    } else {
        console.log("user logged out");
        setupUI();
    }
});

// login
const loginForm = document.querySelector('#login-form');
let failedAttempts = 0;
const maxFailedAttempts = 5;

loginForm.addEventListener('submit', (e) => {
    e.preventDefault();
    
    if (failedAttempts >= maxFailedAttempts) {
        document.getElementById("error-message").innerHTML = "Se han superado los intentos permitidos. Intente más tarde.";
        return;
    }

    // get user info
    const email = loginForm['input-email'].value;
    const password = loginForm['input-password'].value;

    // log the user in
    auth.signInWithEmailAndPassword(email, password).then((cred) => {
        // close the login modal & reset form
        loginForm.reset();
        console.log(email);
        failedAttempts = 0; // reset failed attempts on successful login
        document.getElementById("error-message").innerHTML = ""; // clear error message
    }).catch((error) => {
        const errorCode = error.code;
        const errorMessage = error.message;
        document.getElementById("error-message").innerHTML = errorMessage;
        console.log(errorMessage);
        failedAttempts++;

        if (failedAttempts >= maxFailedAttempts) {
            document.getElementById("error-message").innerHTML = "Se han superado los intentos permitidos. Intente más tarde.";
            // Optionally, you can disable the form here
            loginForm.querySelector('button').disabled = true;
        }
    });
});

// logout
const logout = document.querySelector('#logout-link');
logout.addEventListener('click', (e) => {
    e.preventDefault();
    auth.signOut();
});
