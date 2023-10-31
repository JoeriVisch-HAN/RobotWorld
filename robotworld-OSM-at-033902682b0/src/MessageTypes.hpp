#ifndef MESSAGETYPES_HPP_
#define MESSAGETYPES_HPP_

namespace Messaging
{
	/**
	 * @name The types of messages usable in the application
	 */
	//@{
	enum MessageType
	{
		CommunicationReadError,
		CommunicationWriteError,
		ServerSideExceptionRequest,
		ServerSideExceptionResponse,
		ApplicationExceptionRequest,
		ApplicationExceptionResponse,
		GenericRequest,
		GenericResponse,
		EchoRequest,
		EchoResponse,
		PopulateRequest,
		PopulateResponse,
		StartRequest,
		StartResponse,
		StopRequest,
		StopResponse
	};
	//@}
} /* namespace Messaging */

#endif // MESSAGETYPES_HPP_
