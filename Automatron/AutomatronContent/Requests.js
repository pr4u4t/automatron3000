.pragma library

function sendGetRequest(url, context, callback){
    let request = new XMLHttpRequest();

    request.onreadystatechange = function() {
        if (request.readyState === XMLHttpRequest.DONE) {
            let response = {
                state : request.state,
                status : request.status,
                headers : request.getAllResponseHeaders(),
                contentType : request.responseType,
                content : request.response,
                statusText: request.statusText
            };

            callback(response, context);
        }
    }

    request.open("GET", url);
    request.send();
}
