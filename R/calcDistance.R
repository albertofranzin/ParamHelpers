#' name calcDistance
#'
#' @param par.set [list of \code{\link{ParamSet}} parameters]\cr
#'        Set of parameter configurations, can be a list of configurations
#'        generated from a \code{\link{ParamSet}}, a \code{matrix} or a \code{data.frame}
#'        containing the configurations.
#' @param method [\code{character}]\cr
#'        The metric to be used. Currently, one among \code{gower} and \code{randomForest}.
#'        {gower} is the Gower distance, {randomForest} is based on the proximity value computed by a
#'        random forest.\cr
#' @param data\cr
#'        Data already available.
#' @param model\cr
#'        A statistical model.\cr
#'
#' @return the distance matrix for \code{par.set}.
#'
#' @export
calcDistance <- function(par.set,
			 method = "gower",
                         data = NULL,
                         model = NULL) {

  assert(testList(par.set), testDataFrame(par.set), testMatrix(par.set))
  assertChoice(method, c("gower","randomForest"))

  if (class(par.set) == "list") {
    par.set <- t(as.data.frame(matrix(unlist(par.set), nrow=length(unlist(par.set[1])))))
  }

  if (method == "gower") {
    require("StatMatch")
    distance.matrix <- gower.dist(par.set)
  } else if (method == "randomForest") {
    # now empty...
    distance.matrix <- matrix(0)
  }

  return(distance.matrix)
}

